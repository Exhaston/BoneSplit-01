// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Actors/Player/BSPlayerController.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "BoneSplit/BoneSplit.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Character.h"


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
		if (const float ValueY = Value.Get<FVector2D>().Y * CachedLookSpeed.Y; !FMath::IsNearlyZero(ValueY))
		{
			AddPitchInput(ValueY);
		}
		if (const float ValueX  = Value.Get<FVector2D>().X * CachedLookSpeed.X; !FMath::IsNearlyZero(ValueX))
		{
			AddYawInput(ValueX);
		}
	});	
	
	EnhancedInputComponent->BindActionValueLambda(FreeCamAction, ETriggerEvent::Started, 
	[this](const FInputActionValue& Value)
	{
		if (!GetPawn()) return;
		bFreeCamEnabled = !bFreeCamEnabled;
		if (bFreeCamEnabled)
		{
			GetAbilitySystemComponent()->AddLooseGameplayTag(
				BSTags::Status_Stunned, 1, EGameplayTagReplicationState::CountToOwner);
		}
		else
		{
			GetAbilitySystemComponent()->RemoveLooseGameplayTag(
				BSTags::Status_Stunned, 1, EGameplayTagReplicationState::CountToOwner);
		}
	});
	
	EnhancedInputComponent->BindActionValueLambda(MoveAction, ETriggerEvent::Triggered, 
	[this](const FInputActionValue& Value)
	{
		if (GetPawn())
		{
			const FVector2D InputDir = Value.Get<FVector2D>();
			
			const FVector WorldDir = 
				FVector(InputDir.Y, InputDir.X, 0);
			
			FVector NewDir;
			
			if (bControlDirectionInput)
			{
				FRotator AdjustedControlRotation = GetControlRotation();
				AdjustedControlRotation.Pitch = 0; //Pitch isn't relevant, movement will also stop if looking down or up.
				NewDir = AdjustedControlRotation.RotateVector(WorldDir);
			}
			else
			{
				NewDir = GetPawn()->GetTransform().TransformVector(WorldDir);
			}
			
			GetPawn()->AddMovementInput(NewDir, InputDir.Length());
		}
	});
	
	BindJumpToAction(EnhancedInputComponent, JumpAction);
	
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
	SpecialAction, Special);
	
	BindAbilityToAction(EnhancedInputComponent, 
	SoulAction, Soul);
	
	BindAbilityToAction(EnhancedInputComponent,
	HeadAction, Head);
	
	BindAbilityToAction(EnhancedInputComponent,
	PrimaryArmAction, PrimaryArm);
	
	BindAbilityToAction(EnhancedInputComponent,
	SecondaryArmAction,SecondaryArm);
	
	BindAbilityToAction(EnhancedInputComponent,
	LegsAction, Legs);
}

void ABSPlayerController::Tick(const float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	TurnTimeline.TickTimeline(DeltaSeconds);
	
	//Only need the local owner to buffer their abilities, 
	//server doesn't handle input here
	if (!IsLocalController()) return; 
	
	for (auto& BufferedAbility : BufferedAbilities)
	{
		if (BufferedAbility.TimeRemaining <= 0 || TryActivatePawnAbility(BufferedAbility.AbilityID, false))
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
	
	SetupAsc(InPawn);
}

void ABSPlayerController::AcknowledgePossession(APawn* P)
{
	Super::AcknowledgePossession(P);
	
	SetupAsc(P);
}

void ABSPlayerController::SetupAsc(APawn* InPawn)
{
	if (const IAbilitySystemInterface* AscInterface = Cast<IAbilitySystemInterface>(InPawn))
	{
		UAbilitySystemComponent* Asc = AscInterface->GetAbilitySystemComponent();
		check(Asc);
		CachedAbilitySystemComponent = Asc;
		
		//Technically this isn't required in the controller, as this is done in the player character.
		//Only a failsafe for edge cases with replication race conditions.
		CachedAbilitySystemComponent->InitAbilityActorInfo(InPawn, InPawn);
	}
}

void ABSPlayerController::QuickTurnTimelineTick(const float Alpha)
{
	const FRotator NewRotation = FMath::Lerp(InitialRotation, TargetRotation, Alpha);
	SetControlRotation(NewRotation);
}

bool ABSPlayerController::TryActivatePawnAbility(const int32 ID, const bool bBuffer)
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
			
			if (!Success && bBuffer)
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
		return Ability.AbilityID == ID;
	});
	
	FBSBufferedAbility NewBufferedAbility;
	NewBufferedAbility.AbilityID = ID;
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
		TryActivatePawnAbility(ID, true);
		if (GetAbilitySystemComponent())
		{
			GetAbilitySystemComponent()->PressInputID(ID);
		}
	});
	EnhancedInputComponent->BindActionValueLambda(Action, ETriggerEvent::Completed,
	[this, ID](const FInputActionValue& Value)
	{
		ReleaseAbilityForPawn(ID);
	});
}

void ABSPlayerController::BindJumpToAction(UEnhancedInputComponent* EnhancedInputComponent, UInputAction* Action)
{
	if (!Action) return;
	EnhancedInputComponent->BindActionValueLambda(Action, ETriggerEvent::Started,
	[this](const FInputActionValue& Value)
	{
		if (ACharacter* Character = GetPawn<ACharacter>())
		{
			Character->Jump();
		}
	});
	EnhancedInputComponent->BindActionValueLambda(Action, ETriggerEvent::Completed,
	[this](const FInputActionValue& Value)
	{
		if (ACharacter* Character = GetPawn<ACharacter>())
		{
			Character->StopJumping();
		}
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
