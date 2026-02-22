// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "ClientAuthoritativePlayerController.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/PlayerController.h"
#include "BSPlayerController.generated.h"

class UAbilitySystemComponent;
class UInputAction;
class UInputMappingContext;

USTRUCT(BlueprintType, DisplayName="Buffered Ability", Category="BoneSplit")
struct FBSBufferedAbility
{
	GENERATED_BODY()
	
	FBSBufferedAbility() = default;
	
	UPROPERTY()
	int32 AbilityID = -1;
	
	UPROPERTY()
	float TimeRemaining = 0;
	
	UPROPERTY()
	bool bExpired = false;
	
	bool operator==(const FBSBufferedAbility& Other) const
	{
		return AbilityID == Other.AbilityID;
	}
};
									 
/**
 * Since the pawn can change during different events, we have all the input and buffering setup here directly.
 */                        
UCLASS(DisplayName="Player Controller", Category="BoneSplit", ClassGroup="BoneSplit")
class BONESPLIT_API ABSPlayerController : public AClientAuthoritativePlayerController, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	
	explicit ABSPlayerController(const FObjectInitializer& ObjectInitializer);
	
	virtual void SpawnDefaultHUD() override;
	
	virtual void PreClientTravel(const FString& PendingURL, ETravelType TravelType, bool bIsSeamlessTravel) override;
	
	virtual void GetSeamlessTravelActorList(bool bToEntry, TArray<class AActor*>& ActorList) override;
	
	virtual void SetupInputComponent() override;
	
	virtual void Tick(float DeltaSeconds) override;
	
	virtual void InitPlayerState() override;
	
	virtual void OnRep_PlayerState() override;

protected:
	
	UPROPERTY()
	bool bControlDirectionInput = true;
	
	virtual void SetupAsc(APlayerState* InPS);
	
	UPROPERTY()
	FVector2D CachedLookSpeed = {1, 1};
	
	UPROPERTY()
	FTimeline TurnTimeline;
	
	UFUNCTION()
	void QuickTurnTimelineTick(float Alpha);
	
	virtual bool TryActivatePawnAbility(int32 ID, bool bBuffer = false);
	
	virtual void ReleaseAbilityForPawn(int32 ID);
	
	virtual void BufferAbility(int32 ID);
	
	virtual void BindAbilityToAction(UEnhancedInputComponent* EnhancedInputComponent, UInputAction* Action, int32 ID, bool bCanReTrigger = false);
	
	virtual void BindJumpToAction(UEnhancedInputComponent* EnhancedInputComponent, UInputAction* Action);
	
	//Mainly implemented through IAbilitySystemInterface for UI easy access.
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	//The amount of time an ability can attempt to retrigger after a failed activation.
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
	UInputAction* CharacterPaneAction;
	
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
	
	UPROPERTY(Transient)
	TWeakObjectPtr<UAbilitySystemComponent> CachedAbilitySystemComponent;
	
	UPROPERTY()
	TArray<FBSBufferedAbility> BufferedAbilities;
};
