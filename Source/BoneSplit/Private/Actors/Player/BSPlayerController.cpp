// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Actors/Player/BSPlayerController.h"

#include "AbilityBufferComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "CharacterAbilitySystem.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Actors/Player/BSPlayerState.h"
#include "BoneSplit/BoneSplit.h"
#include "Components/TimelineComponent.h"
#include "Components/AbilitySystem/BSAbilitySystemComponent.h"
#include "Components/InteractionSystem/BSInteractionComponent.h"
#include "Equipment/BSEquipmentComponent.h"
#include "GameFramework/Character.h"
#include "GameInstance/BSLoadingScreenSubsystem.h"
#include "Widgets/CommonActivatableWidgetContainer.h"
#include "Widgets/Base/BSPauseMenu.h"
#include "Widgets/HUD/BSCharacterPane.h"
#include "Widgets/HUD/BSGameplayRootWidget.h"
#include "Widgets/HUD/BSWDamageNumber.h"
#include "Widgets/HUD/BSWHud.h"


ABSPlayerController::ABSPlayerController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	AbilityBufferComponent = CreateDefaultSubobject<UAbilityBufferComponent>("AbilityBufferComponent");
}

void ABSPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
	check(EnhancedInputComponent);
	
	AbilityBufferComponent->BindAbilityInputActions(EnhancedInputComponent);
	
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
		TogglePauseMenu();
	});	
	
	EnhancedInputComponent->BindActionValueLambda(CharacterPaneAction, ETriggerEvent::Started, 
	[this](const FInputActionValue& Value)
	{
		ToggleCharacterPane();
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
	
}

void ABSPlayerController::OnUnPossess()
{
	if (APawn* PawnBeingUnpossessed = GetPawn())
	{
		if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(PlayerState))
		{
			if (ASC->GetAvatarActor() == PawnBeingUnpossessed)
			{
				ASC->SetAvatarActor(nullptr);
			}
		}
	}
	
	Super::OnUnPossess();
}

void ABSPlayerController::Tick(const float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	TurnTimeline.TickTimeline(DeltaSeconds);
}

void ABSPlayerController::QuickTurnTimelineTick(const float Alpha)
{
	const FRotator NewRotation = FMath::Lerp(InitialRotation, TargetRotation, Alpha);
	SetControlRotation(NewRotation);
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
	return CharacterAbilitySystem;
}

UCommonActivatableWidgetStack* ABSPlayerController::GetWidgetStack()
{
	return GameplayRootWidgetInstance ? GameplayRootWidgetInstance->WidgetStack : nullptr;
}

UCharacterAbilitySystem* ABSPlayerController::GetCharacterAbilitySystem()
{
	return CharacterAbilitySystem;
}

void ABSPlayerController::OnDamageDealt(UAbilitySystemComponent* InstigatorAsc, UAbilitySystemComponent* TargetAsc,
	FGameplayTagContainer EffectTags, float BaseDamage, float TotalDamage)
{
	if (!IsLocalController() || !GetPawn() || !TargetAsc || !TargetAsc->GetAvatarActor()) return;
	
	//TODO: Convert to be added to a subwidget, so we can just toggle the owners visiblity instead of individual numbers.
	bool bBlocked = false;
	
	if (GetPawn() == TargetAsc->GetAvatarActor())
	{
		if (!BSConsoleVariables::CVarBSShowIncomingDamageNumbers.GetValueOnGameThread()) bBlocked = true;
	}
	else
	{
		if (!BSConsoleVariables::CVarBSShowDamageNumbers.GetValueOnGameThread()) bBlocked = true;
	}
	
	if (bBlocked) return;
	
	UBSWDamageNumber* NewDamageNumber = CreateWidget<UBSWDamageNumber>(this, FloatingDamageNumberClass);
	NewDamageNumber->AddToPlayerScreen();
	NewDamageNumber->InitializeDamageNumber(TargetAsc->GetAvatarActor(), TotalDamage, EffectTags);
}

void ABSPlayerController::SpawnFloatingNumber(UAbilitySystemComponent* InstigatorAsc,
	UAbilitySystemComponent* TargetAsc, FGameplayTagContainer EffectTags, float Number)
{
}

void ABSPlayerController::AddPlayerUI()
{
	if (GameplayRootWidgetInstance)
	{
		GameplayRootWidgetInstance->RemoveFromParent();
		GameplayRootWidgetInstance = nullptr;
	}
		
	GameplayRootWidgetInstance = CreateWidget<UBSGameplayRootWidget>(this, GameplayRootWidgetClass);
	GameplayRootWidgetInstance->AddToPlayerScreen();
	
	UBSWHud* HUDWidget = GameplayRootWidgetInstance->WidgetStack->AddWidget<UBSWHud>(HudWidgetClass);
	HUDWidget->InitializePlayerHUD(CharacterAbilitySystem);
}

void ABSPlayerController::InitializeWithAbilitySystem(UCharacterAbilitySystem* InCharacterAbilitySystem)
{
	CharacterAbilitySystem = InCharacterAbilitySystem;
	CharacterAbilitySystem->OnDamageDealt.AddDynamic(this, &ABSPlayerController::OnDamageDealt);
	AbilityBufferComponent->SetAbilitySystemComponent(CharacterAbilitySystem);
	
	if (IsLocalController() && !IsRunningDedicatedServer())
	{
		AddPlayerUI();
	}
}

UBSEquipmentComponent* ABSPlayerController::GetEquipmentComponent() const
{
	return GetPlayerState<ABSPlayerState>()->GetEquipmentComponent();
}

void ABSPlayerController::ApplyEquipment(const FBSEquipPickupInfo& Pickup)
{
	GetEquipmentComponent()->Server_RequestEquipFromDrop(Pickup);
}

void ABSPlayerController::DestroyPlayerUI()
{
	if (!GameplayRootWidgetInstance) return;
	GameplayRootWidgetInstance->RemoveFromParent();
	GameplayRootWidgetInstance = nullptr;
}

void ABSPlayerController::SpawnDamageNumber(FGameplayEventData EventData)
{
	//TODO; Maybe pool these somehow
	if (!IsLocalController() || !GetPawn() || !EventData.Target) return;
	
	bool bBlocked = false;
	
	if (GetPawn() == EventData.Target)
	{
		if (!BSConsoleVariables::CVarBSShowIncomingDamageNumbers.GetValueOnGameThread()) bBlocked = true;
	}
	else
	{
		if (!BSConsoleVariables::CVarBSShowDamageNumbers.GetValueOnGameThread()) bBlocked = true;
	}
	
	if (bBlocked) return;
	
	UBSWDamageNumber* NewDamageNumber = CreateWidget<UBSWDamageNumber>(this, FloatingDamageNumberClass);
	NewDamageNumber->AddToPlayerScreen();
	NewDamageNumber->InitializeDamageNumber(EventData);
}

void ABSPlayerController::TogglePauseMenu()
{
	
	if (!IsLocalController()) return;
	if (GameplayRootWidgetInstance && PauseMenuClass)
	{
		if (auto* FoundWidget = 
			GameplayRootWidgetInstance->WidgetStack->GetWidgetList().FindByPredicate(
			[this](const UCommonActivatableWidget* Widget)
			{
				return Widget && Widget->GetClass() == PauseMenuClass;
			}))
		{
			if (*FoundWidget)
			{
				(*FoundWidget)->DeactivateWidget();
			}
		}
		else if (!IsInPauseMenu())
		{
			GameplayRootWidgetInstance->WidgetStack->AddWidget(PauseMenuClass);
		}
	}
}

void ABSPlayerController::ToggleCharacterPane()
{
	if (!IsLocalController()) return;
	if (GameplayRootWidgetInstance && CharacterPaneClass)
	{
		if (auto* FoundWidget = 
			GameplayRootWidgetInstance->WidgetStack->GetWidgetList().FindByPredicate(
			[this](const UCommonActivatableWidget* Widget)
			{
				return Widget && Widget->GetClass() == CharacterPaneClass;
			}))
		{
			if (*FoundWidget)
			{
				(*FoundWidget)->DeactivateWidget();
			}
		}
		else if (!IsInPauseMenu())
		{
			GameplayRootWidgetInstance->WidgetStack->AddWidget(CharacterPaneClass);
		}
	}
}

bool ABSPlayerController::IsInPauseMenu()
{
	if (IsLocalController())
	{
		if (GameplayRootWidgetInstance && PauseMenuClass)
		{
			return GameplayRootWidgetInstance->WidgetStack->GetWidgetList().ContainsByPredicate(
				[this](const UCommonActivatableWidget* Widget)
			{
				return Widget && Widget->GetClass() == PauseMenuClass;
			});
		}
	}
	return false;
}
