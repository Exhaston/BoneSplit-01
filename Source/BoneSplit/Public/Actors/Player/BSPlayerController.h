// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ClientAuthoritativePlayerController.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/PlayerController.h"
#include "BSPlayerController.generated.h"

class UAbilitySystemComponent;
class UInputAction;
class UInputMappingContext;

USTRUCT(BlueprintType)
struct FBSBufferedAbility
{
	GENERATED_BODY()
	
	FBSBufferedAbility() = default;
	
	UPROPERTY()
	int32 ID = -1;
	
	UPROPERTY()
	float TimeRemaining = 0;
	
	UPROPERTY()
	bool bExpired = false;
	
	bool operator==(const FBSBufferedAbility& Other) const
	{
		return ID == Other.ID;
	}
};

/**
 * 
 */
UCLASS()
class BONESPLIT_API ABSPlayerController : public AClientAuthoritativePlayerController
{
	GENERATED_BODY()
	
public:
	explicit ABSPlayerController(const FObjectInitializer& ObjectInitializer);
	
	virtual void SetupInputComponent() override;
	
	virtual void Tick(float DeltaSeconds) override;
	
	virtual void OnPossess(APawn* InPawn) override;
	
	virtual void AcknowledgePossession(APawn* P) override;
	
protected:
	
	UPROPERTY()
	FVector2D CachedLookSpeed = {1, 1};
	
	UPROPERTY()
	FTimeline TurnTimeline;
	
	UFUNCTION()
	void QuickTurnTimelineTick(float Alpha);
	
	virtual bool TryActivatePawnAbility(int32 ID);
	
	virtual void ReleaseAbilityForPawn(int32 ID);
	
	virtual void BufferAbility(int32 ID);
	
	virtual void BindAbilityToAction(UEnhancedInputComponent* EnhancedInputComponent, UInputAction* Action, int32 ID);
	
	UAbilitySystemComponent* GetAbilitySystemComponent() const;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Input|Buffering")
	float BufferTime = 0.5;
	
	UPROPERTY()
	FRotator InitialRotation;

	UPROPERTY()
	FRotator TargetRotation;
	
	UPROPERTY()
	bool bFreeCamEnabled = false;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Input")
	float QuickTurnTime = 0.25;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Input")
	UInputMappingContext* MappingContext;
	
	UPROPERTY(BLueprintReadOnly, EditDefaultsOnly, Category="Input|Actions")
	UInputAction* PauseAction;
	
	UPROPERTY(BLueprintReadOnly, EditDefaultsOnly, Category="Input|Actions")
	UInputAction* MoveAction;
	
	UPROPERTY(BLueprintReadOnly, EditDefaultsOnly, Category="Input|Actions")
	UInputAction* LookAction;
	
	UPROPERTY(BLueprintReadOnly, EditDefaultsOnly, Category="Input|Actions")
	UInputAction* QuickTurnAction;
	
	UPROPERTY(BLueprintReadOnly, EditDefaultsOnly, Category="Input|Actions")
	UInputAction* FreeCamAction;
	
	UPROPERTY(BLueprintReadOnly, EditDefaultsOnly, Category="Input|Actions")
	UInputAction* InteractAction;
	
	UPROPERTY(BLueprintReadOnly, EditDefaultsOnly, Category="Input|Actions")
	UInputAction* JumpAction;
	
	UPROPERTY(BLueprintReadOnly, EditDefaultsOnly, Category="Input|Actions")
	UInputAction* HeadAction;
	
	UPROPERTY(BLueprintReadOnly, EditDefaultsOnly, Category="Input|Actions")
	UInputAction* PrimaryArmAction;
	
	UPROPERTY(BLueprintReadOnly, EditDefaultsOnly, Category="Input|Actions")
	UInputAction* SecondaryArmAction;
	
	UPROPERTY(BLueprintReadOnly, EditDefaultsOnly, Category="Input|Actions")
	UInputAction* LegsAction;
	
	UPROPERTY(BLueprintReadOnly, EditDefaultsOnly, Category="Input|Actions")
	UInputAction* SoulAction;
	
	UPROPERTY(BLueprintReadOnly, EditDefaultsOnly, Category="Input|Actions")
	UInputAction* SpecialAction;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="UI")
	TSubclassOf<UUserWidget> HudWidget;
	
	UPROPERTY(Transient)
	TWeakObjectPtr<UAbilitySystemComponent> CachedAbilitySystemComponent;
	
	UPROPERTY()
	TArray<FBSBufferedAbility> BufferedAbilities;
};
