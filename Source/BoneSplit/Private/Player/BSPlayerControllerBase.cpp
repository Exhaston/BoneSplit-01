// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Player/BSPlayerControllerBase.h"

#include "AbilityBufferComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "CharacterAbilitySystem.h"
#include "EnhancedInputComponent.h"
#include "BoneSplit/BoneSplit.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/PlayerState.h"
#include "Player/HUD/BSHudComponent.h"
#include "Player/BSPlayerCharacterBase.h"

namespace BSConsoleVariables
{
	TAutoConsoleVariable<float> CVarBSLookSensitivityX(
	TEXT("BS.Controls.MouseSensitivity.X"),
	1,
	TEXT("1 = Default"),
	ECVF_Default);

	TAutoConsoleVariable<float> CVarBSLookSensitivityY(
	TEXT("BS.Controls.MouseSensitivity.Y"),
	1,
	TEXT("1 = Default"),
	ECVF_Default);
	
	TAutoConsoleVariable<bool> CVarInvertLookY(
		TEXT("BS.Controls.InvertAxisVertical"),
		false,
		TEXT("0 = Default"),
		ECVF_Default);
}

ABSPlayerControllerBase::ABSPlayerControllerBase(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	AbilityBufferComponent = CreateDefaultSubobject<UAbilityBufferComponent>("AbilityBufferComponent");
	HudComponent = CreateDefaultSubobject<UBSHudComponent>("HudComponent");
}

void ABSPlayerControllerBase::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
	check(EnhancedInputComponent);
	
	AbilityBufferComponent->BindAbilityInputActions(EnhancedInputComponent);
	
	// =================================================================================================================
	// Movement and Looking
	// =================================================================================================================
	
	CachedLookSensitivityX = BSConsoleVariables::CVarBSLookSensitivityX.GetValueOnGameThread();
	CachedLookSensitivityY = BSConsoleVariables::CVarBSLookSensitivityY.GetValueOnGameThread();
	
	BSConsoleVariables::CVarBSLookSensitivityY->OnChangedDelegate().AddWeakLambda(
	this, [this] (const IConsoleVariable* Variable)
	{
		CachedLookSensitivityY = Variable->GetFloat();
	});	
	
	BSConsoleVariables::CVarBSLookSensitivityX->OnChangedDelegate().AddWeakLambda(
	this, [this] (const IConsoleVariable* Variable)
	{
		CachedLookSensitivityX = Variable->GetFloat();
	});
	
	bInvertY = BSConsoleVariables::CVarInvertLookY.GetValueOnGameThread();
	BSConsoleVariables::CVarInvertLookY->OnChangedDelegate().AddWeakLambda(
	this, [this] (const IConsoleVariable* Variable)
	{
		bInvertY = Variable->GetBool();
	});
	
	EnhancedInputComponent->BindActionValueLambda(LookAction, ETriggerEvent::Triggered, 
	[this](const FInputActionValue& Value)
	{
		if (!GetPawn()) return;
		if (const float ValueY = Value.Get<FVector2D>().Y * CachedLookSensitivityY * (bInvertY ? -1 : 1); !FMath::IsNearlyZero(ValueY))
		{
			AddPitchInput(ValueY);
		}
		if (const float ValueX  = Value.Get<FVector2D>().X * CachedLookSensitivityX; !FMath::IsNearlyZero(ValueX))
		{
			AddYawInput(ValueX);
		}
	});	
	
	EnhancedInputComponent->BindActionValueLambda(PauseAction, ETriggerEvent::Started, 
	[this](const FInputActionValue& Value)
	{
		HudComponent->SetPauseMenuActive();
	});	
	
	EnhancedInputComponent->BindActionValueLambda(CharacterPaneAction, ETriggerEvent::Started, 
	[this](const FInputActionValue& Value)
	{
		//HudComponent->SetPauseMenuActive(false);
	});
	
	EnhancedInputComponent->BindActionValueLambda(MoveAction, ETriggerEvent::Triggered, 
	[this](const FInputActionValue& Value)
	{
		APawn* CurrentPawn = GetPawn();
		if (!CurrentPawn) return;
		
		const FVector2D InputDir = Value.Get<FVector2D>();
		const FVector WorldDir = FVector(InputDir.Y, InputDir.X, 0);

		FRotator AdjustedControlRotation = GetControlRotation();
		AdjustedControlRotation.Pitch = 0; //Pitch isn't relevant, movement will also stop if looking down or up.
		const FVector NewDir = AdjustedControlRotation.RotateVector(WorldDir);
			
		CurrentPawn->AddMovementInput(NewDir, InputDir.Length());
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
}

void ABSPlayerControllerBase::Tick(const float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	TurnTimeline.TickTimeline(DeltaSeconds);
}

UAbilitySystemComponent* ABSPlayerControllerBase::GetAbilitySystemComponent() const
{
	if (const IAbilitySystemInterface* PSAscI = GetPlayerState<IAbilitySystemInterface>())
	{
		return PSAscI->GetAbilitySystemComponent();
	}
	return nullptr;
}

void ABSPlayerControllerBase::AcknowledgePossession(APawn* P)
{
	Super::AcknowledgePossession(P);
	
	ABSPlayerCharacterBase* PlayerCharacterBase = Cast<ABSPlayerCharacterBase>(P);
	
	if (!PlayerCharacterBase)
	{
		UE_LOG(BoneSplit, Error, TEXT("BS Player Controller used with incompatible pawn."));
		return;
	}
	
	if (PlayerCharacterBase->GetIsAbilitySystemInitComplete())
	{
		OnPawnAbilitySystemInit();
	}
	else
	{
		PlayerCharacterBase->GetOnAbilitySystemInitializedDelegate().AddUObject(
			this, &ABSPlayerControllerBase::OnPawnAbilitySystemInit);
	}
}

void ABSPlayerControllerBase::OnUnPossess()
{
	if (const APawn* PawnBeingUnpossessed = GetPawn())
	{
		if (UAbilitySystemComponent* Asc = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(PlayerState))
		{
			if (Asc->GetAvatarActor() == PawnBeingUnpossessed)
			{
				Asc->SetAvatarActor(nullptr);
				Asc->RefreshAbilityActorInfo();
			}
		}
	}
	Super::OnUnPossess();                              
}

void ABSPlayerControllerBase::OnPawnAbilitySystemInit()
{
	ABSPlayerCharacterBase* PlayerCharacterBase = GetPawn<ABSPlayerCharacterBase>();
	if (!PlayerCharacterBase) return;
	
	PlayerCharacterBase->GetOnAbilitySystemInitializedDelegate().RemoveAll(this);
	
	if (IsLocalController())
	{
		AbilityBufferComponent->SetAbilitySystemComponent(Cast<UCharacterAbilitySystem>(GetAbilitySystemComponent()));
		UE_LOG(BoneSplit, Display, TEXT("Player initialized successfully. Adding UI."));
		HudComponent->SetupHud(this);
		
		//TODO: Remove loading screen if any
	}
}

void ABSPlayerControllerBase::QuickTurnTimelineTick(const float Alpha)
{
	const FRotator NewRotation = FMath::Lerp(InitialRotation, TargetRotation, Alpha);
	SetControlRotation(NewRotation);
}
