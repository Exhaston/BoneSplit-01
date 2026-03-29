// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "ClientAuthoritativePlayerController.h"
#include "Components/TimelineComponent.h"
#include "BSPlayerControllerBase.generated.h"

class UBSHudComponent;
class UInputAction;
class UAbilityBufferComponent;

namespace BSConsoleVariables
{
	extern TAutoConsoleVariable<float> CVarBSLookSensitivityX;
	extern TAutoConsoleVariable<float> CVarBSLookSensitivityY;
	extern TAutoConsoleVariable<bool> CVarInvertLookY;
}

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType, Abstract, DisplayName="BS Player Controller Base")
class BONESPLIT_API ABSPlayerControllerBase : public AClientAuthoritativePlayerController, 
public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:
	
	explicit ABSPlayerControllerBase(const FObjectInitializer& ObjectInitializer);
	
	virtual void SetupInputComponent() override;
	
	virtual void Tick(float DeltaSeconds) override;
	
	//May return null, especially during initialization. Should be valid whenever we have a valid player state.
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	virtual void AcknowledgePossession(APawn* P) override;
	
	virtual void OnUnPossess() override;
	
	UFUNCTION()
	void OnPawnAbilitySystemInit();
	
	UBSHudComponent* GetHudComponent() const { return HudComponent; }
	
protected:
	
	UPROPERTY(BLueprintReadOnly, EditDefaultsOnly, Category="Input|Actions")
	UInputAction* PauseAction;
	
	UPROPERTY(BLueprintReadOnly, EditDefaultsOnly, Category="Input|Actions")
	UInputAction* CharacterPaneAction;
	
	UPROPERTY(BLueprintReadOnly, EditDefaultsOnly, Category="Input|Actions")
	UInputAction* MoveAction;
	
	UPROPERTY(BLueprintReadOnly, EditDefaultsOnly, Category="Input|Actions")
	UInputAction* LookAction;
	
	UPROPERTY(BLueprintReadOnly, EditDefaultsOnly, Category="Input|Actions")
	UInputAction* QuickTurnAction;
	
	UPROPERTY()
	FTimeline TurnTimeline;
	
	UFUNCTION()
	void QuickTurnTimelineTick(float Alpha);
	
	FRotator InitialRotation;
	
	FRotator TargetRotation;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Input")
	float QuickTurnTime = 0.25;
	
	float CachedLookSensitivityX = 1;
	float CachedLookSensitivityY = 1;
	bool bInvertY = false;
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<UBSHudComponent> HudComponent;
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<UAbilityBufferComponent> AbilityBufferComponent;
};
