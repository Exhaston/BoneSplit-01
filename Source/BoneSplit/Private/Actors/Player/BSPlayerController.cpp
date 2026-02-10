// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Actors/Player/BSPlayerController.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Actors/Player/BSLocalSaveSubsystem.h"
#include "Actors/Player/BSPlayerState.h"
#include "BoneSplit/BoneSplit.h"
#include "Components/TimelineComponent.h"
#include "Components/InteractionSystem/BSInteractionComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerState.h"
#include "GameInstance/BSLoadingScreenSubsystem.h"
#include "Widgets/BSLocalWidgetSubsystem.h"
#include "Widgets/Base/BSPauseMenu.h"
#include "Widgets/HUD/BSCharacterPane.h"


ABSPlayerController::ABSPlayerController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	
}

void ABSPlayerController::PreClientTravel(const FString& PendingURL, ETravelType TravelType, bool bIsSeamlessTravel)
{
	if (GetAbilitySystemComponent() && GetGameInstance())
	{
		GetGameInstance()->GetSubsystem<UBSLocalSaveSubsystem>()->SaveAscDataSync(
			this, GetAbilitySystemComponent());
	}
	
	Super::PreClientTravel(PendingURL, TravelType, bIsSeamlessTravel);
	
	if (UBSLoadingScreenSubsystem* LoadingScreenSubsystem = GetLocalPlayer()->GetSubsystem<UBSLoadingScreenSubsystem>(); GetLocalPlayer()->IsPrimaryPlayer())
	{
		LoadingScreenSubsystem->AddLoadingScreen(nullptr);
	}
}

void ABSPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
	check(EnhancedInputComponent);
	
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
	
	EnhancedInputComponent->BindActionValueLambda(PauseAction, ETriggerEvent::Started, 
	[this](const FInputActionValue& Value)
	{
		UBSLocalWidgetSubsystem* LocalWidgetSubsystem = GetLocalPlayer()->GetSubsystem<UBSLocalWidgetSubsystem>();
		if (LocalWidgetSubsystem->bIsPaused) return;
		LocalWidgetSubsystem->AddWidgetToStack<UBSPauseMenu>(LocalWidgetSubsystem->PauseMenuWidgetClass);
	});	
	
	EnhancedInputComponent->BindActionValueLambda(CharacterPaneAction, ETriggerEvent::Started, 
	[this](const FInputActionValue& Value)
	{
		UBSLocalWidgetSubsystem* LocalWidgetSubsystem = GetLocalPlayer()->GetSubsystem<UBSLocalWidgetSubsystem>();
		if (LocalWidgetSubsystem->IsWidgetActive(UBSCharacterPane::StaticClass()))
		{
			LocalWidgetSubsystem->RootWidgetInstance->WidgetStack->GetActiveWidget()->DeactivateWidget();
		}
		else
		{
			LocalWidgetSubsystem->AddWidgetToStack<UBSCharacterPane>(LocalWidgetSubsystem->CharacterPaneWidgetClass);
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
	
	EnhancedInputComponent->BindActionValueLambda(InteractAction, ETriggerEvent::Started, 
	[this](const FInputActionValue& Value)
	{
		if (GetPawn() && !GetPawn()->IsActorBeingDestroyed())
		{
			if (UBSInteractionComponent* InteractionComponent = 
				GetPawn()->GetComponentByClass<UBSInteractionComponent>())
			{
				InteractionComponent->TryInteract();
			}
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
	SpecialAction, 6);
	
	BindAbilityToAction(EnhancedInputComponent, 
	SoulAction, 5);
	
	BindAbilityToAction(EnhancedInputComponent,
	HeadAction, 3);
	
	BindAbilityToAction(EnhancedInputComponent,
	PrimaryArmAction, 1, true);
	
	BindAbilityToAction(EnhancedInputComponent,
	SecondaryArmAction,2);
	
	BindAbilityToAction(EnhancedInputComponent,
	LegsAction, 4);
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

void ABSPlayerController::InitPlayerState()
{
	Super::InitPlayerState();
	SetupAsc(GetPlayerState<APlayerState>());
}

void ABSPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	SetupAsc(GetPlayerState<APlayerState>());
}

void ABSPlayerController::SetupAsc(APlayerState* InPS)
{
	if (const IAbilitySystemInterface* AscInterface = Cast<IAbilitySystemInterface>(InPS))
	{
		UAbilitySystemComponent* Asc = AscInterface->GetAbilitySystemComponent();
		check(Asc);
		CachedAbilitySystemComponent = Asc;
		
		//Technically this isn't required in the controller, as this is done in the player character.
		//Only a failsafe for edge cases with replication race conditions.
		CachedAbilitySystemComponent->InitAbilityActorInfo(InPS, InPS->GetPawn());
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

void ABSPlayerController::BindAbilityToAction(UEnhancedInputComponent* EnhancedInputComponent, UInputAction* Action, int32 ID, const bool bCanReTrigger)
{
	if (!Action) return;
	EnhancedInputComponent->BindActionValueLambda(Action, bCanReTrigger ? ETriggerEvent::Triggered : ETriggerEvent::Started,
	[this, ID, bCanReTrigger](const FInputActionValue& Value)
	{
		TryActivatePawnAbility(ID, !bCanReTrigger);
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
