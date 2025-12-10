// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Actors/Player/BSPlayerController.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "BoneSplit/BoneSplit.h"
#include "Components/TimelineComponent.h"


ABSPlayerController::ABSPlayerController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

void ABSPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	UEnhancedInputLocalPlayerSubsystem* Subsystem =
	GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	
	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
	check(EnhancedInputComponent);
	
	Subsystem->AddMappingContext(MappingContext, 1);
	
	// =================================================================================================================
	// Movement and Looking
	// =================================================================================================================
	
	CachedLookSpeed.X = BSConsoleVariables::CVarBSLookSensitivityX.GetValueOnGameThread();
	CachedLookSpeed.Y = BSConsoleVariables::CVarBSLookSensitivityY.GetValueOnGameThread();
	
	BSConsoleVariables::CVarBSLookSensitivityY->OnChangedDelegate().AddWeakLambda(
	this, [this] (const IConsoleVariable* Variable)
	{
		CachedLookSpeed.Y = Variable->GetFloat();
	});	
	BSConsoleVariables::CVarBSLookSensitivityX->OnChangedDelegate().AddWeakLambda(
	this, [this] (const IConsoleVariable* Variable)
	{
		CachedLookSpeed.X = Variable->GetFloat();
	});
	
	EnhancedInputComponent->BindActionValueLambda(LookAction, ETriggerEvent::Triggered, 
	[this](const FInputActionValue& Value)
	{
		AddPitchInput(-Value.Get<FVector2D>().Y * CachedLookSpeed.Y);
		AddYawInput(Value.Get<FVector2D>().X * CachedLookSpeed.X);
	});	
	EnhancedInputComponent->BindActionValueLambda(FreeCamAction, ETriggerEvent::Started, 
	[this](const FInputActionValue& Value)
	{
		bFreeCamEnabled = !bFreeCamEnabled;
		if (bFreeCamEnabled)
		{
			GetPawn()->bUseControllerRotationYaw = false;
		}
		else
		{
			//Reset first to make sure movement is reliable to input direction
			SetControlRotation(GetPawn()->GetActorRotation()); 
			GetPawn()->bUseControllerRotationYaw = true;
		}
	});
	
	EnhancedInputComponent->BindActionValueLambda(MoveAction, ETriggerEvent::Triggered, 
	[this](const FInputActionValue& Value)
	{
		if (GetPawn())
		{
			const FVector2D InputDir = Value.Get<FVector2D>();
			
			const FVector WorldDir = 
				GetPawn()->GetTransform().TransformVector(
					FVector(InputDir.Y, InputDir.X, 0)).GetSafeNormal();
			
			GetPawn()->AddMovementInput(WorldDir, InputDir.Length());
		}
	});
	
	// =================================================================================================================
	// Quick Turn Timeline
	// =================================================================================================================
	
	FOnTimelineFloat ProgressFunction;
	ProgressFunction.BindUFunction(this, FName("QuickTurnTimelineTick"));
	
	UCurveFloat* TurnCurve = NewObject<UCurveFloat>();
	FRichCurve* RichCurve = &TurnCurve->FloatCurve;
	RichCurve->AddKey(0.0f, 0.0f, false);
	RichCurve->AddKey(QuickTurnTime, 1.0f, false);
	for (auto& Key : RichCurve->Keys)
	{
		Key.InterpMode = RCIM_Cubic;
	}
	
	TurnTimeline.AddInterpFloat(TurnCurve, ProgressFunction);
	TurnTimeline.SetLooping(false);
	TurnTimeline.SetTimelineLengthMode(TL_LastKeyFrame);	
	
	EnhancedInputComponent->BindActionValueLambda(QuickTurnAction, ETriggerEvent::Started, 
	[this](const FInputActionValue& Value)
	{
		InitialRotation = GetControlRotation();
		TargetRotation = InitialRotation + FRotator(0.f, 180.f, 0.f);
		TurnTimeline.PlayFromStart();
	});
	
	// =================================================================================================================
	// Abilities
	// =================================================================================================================
	
	BindAbilityToAction(EnhancedInputComponent, 
	JumpAction, static_cast<int32>(EBSAbilityInputID::Jump));
	
	BindAbilityToAction(EnhancedInputComponent, 
	SpecialAction, static_cast<int32>(EBSAbilityInputID::Special));
	
	BindAbilityToAction(EnhancedInputComponent, 
	SoulAction, static_cast<int32>(EBSAbilityInputID::Soul));
	
	BindAbilityToAction(EnhancedInputComponent,
	HeadAction, static_cast<int32>(EBSAbilityInputID::Head));
	
	BindAbilityToAction(EnhancedInputComponent,
	PrimaryArmAction, static_cast<int32>(EBSAbilityInputID::PrimaryArm));
	
	BindAbilityToAction(EnhancedInputComponent,
	SecondaryArmAction, static_cast<int32>(EBSAbilityInputID::SecondaryArm));
	
	BindAbilityToAction(EnhancedInputComponent,
	LegsAction, static_cast<int32>(EBSAbilityInputID::Legs)); 
}

void ABSPlayerController::Tick(const float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	TurnTimeline.TickTimeline(DeltaSeconds);
	
	for (auto& BufferedAbility : BufferedAbilities)
	{
		if (BufferedAbility.TimeRemaining <= 0 || TryActivatePawnAbility(BufferedAbility.ID))
		{
			BufferedAbility.bExpired = true;
		}
		else
		{
			BufferedAbility.TimeRemaining -= DeltaSeconds;
		}
	}

	BufferedAbilities.RemoveAll([](const FBSBufferedAbility& Ability)
	{
		return Ability.bExpired;
	});
}

void ABSPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	if (const IAbilitySystemInterface* AscInterface = Cast<IAbilitySystemInterface>(InPawn))
	{
		UAbilitySystemComponent* Asc = AscInterface->GetAbilitySystemComponent();
		check(Asc);
		CachedAbilitySystemComponent = Asc;
	}
}

void ABSPlayerController::AcknowledgePossession(APawn* P)
{
	Super::AcknowledgePossession(P);
	
	if (const IAbilitySystemInterface* AscInterface = Cast<IAbilitySystemInterface>(P))
	{
		UAbilitySystemComponent* Asc = AscInterface->GetAbilitySystemComponent();
		check(Asc);
		CachedAbilitySystemComponent = Asc;
	}
}

void ABSPlayerController::QuickTurnTimelineTick(const float Alpha)
{
	const FRotator NewRotation = FMath::Lerp(InitialRotation, TargetRotation, Alpha);
	SetControlRotation(NewRotation);
}

bool ABSPlayerController::TryActivatePawnAbility(const int32 ID)
{
	if (!GetPawn()) return false;

	if (UAbilitySystemComponent* Asc = GetAbilitySystemComponent())
	{
		if (const FGameplayAbilitySpec* Spec = Asc->FindAbilitySpecFromInputID(ID))
		{
			bool Success = false;
			if (const UGameplayAbility* AbilityInstance = Spec->GetPrimaryInstance())
			{
				Success = Asc->TryActivateAbility(AbilityInstance->GetCurrentAbilitySpecHandle());
			}
			
			if (!Success)
			{
				BufferAbility(ID);
			}
			return Success;
		}
	}
	return false;
}

void ABSPlayerController::ReleaseAbilityForPawn(const int32 ID)
{
	if (UAbilitySystemComponent* Asc = GetAbilitySystemComponent())
	{
		Asc->ReleaseInputID(ID);
	}
}

void ABSPlayerController::BufferAbility(int32 ID)
{
	BufferedAbilities.RemoveAll([ID](const FBSBufferedAbility& Ability)
	{
		return Ability.ID == ID;
	});
	
	FBSBufferedAbility NewBufferedAbility;
	NewBufferedAbility.ID = ID;
	NewBufferedAbility.TimeRemaining = BufferTime;
	NewBufferedAbility.bExpired = false;
	BufferedAbilities.Add(NewBufferedAbility);
}

void ABSPlayerController::BindAbilityToAction(UEnhancedInputComponent* EnhancedInputComponent, UInputAction* Action, int32 ID)
{
	if (!Action) return;
	EnhancedInputComponent->BindActionValueLambda(Action, ETriggerEvent::Started,
	[this, ID](const FInputActionValue& Value)
	{
		TryActivatePawnAbility(ID);
	});
	EnhancedInputComponent->BindActionValueLambda(Action, ETriggerEvent::Completed,
	[this, ID](const FInputActionValue& Value)
	{
		ReleaseAbilityForPawn(ID);
	});
}

UAbilitySystemComponent* ABSPlayerController::GetAbilitySystemComponent() const
{
	if (!GetPawn()) return nullptr;
	if (CachedAbilitySystemComponent.IsValid())
	{
		return CachedAbilitySystemComponent.Get();
	}
	
	return nullptr;
}
