// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "ClientAuthoritativePlayerController.h"
#include "GameplayTagContainer.h"
#include "Components/TimelineComponent.h"
#include "Equipment/BSEquipmentInterface.h"
#include "GameFramework/PlayerController.h"
#include "BSPlayerController.generated.h"

class UAbilityBufferComponent;
struct FGameplayTagContainer;
class UCharacterAbilitySystem;
class UBSAbilitySystemComponent;
class UCommonActivatableWidgetStack;
struct FGameplayEventData;
class UBSGameplayRootWidget;
class UBSWDamageNumber;
class UBSPauseMenu;
class UBSCharacterPane;
class UBSWHud;
class UAbilitySystemComponent;
class UInputAction;
class UInputMappingContext;
									 
/**
 * Since the pawn can change during different events, we have all the input and buffering setup here directly.
 */                        
UCLASS(DisplayName="Player Controller", Category="BoneSplit", ClassGroup="BoneSplit")
class BONESPLIT_API ABSPlayerController : public AClientAuthoritativePlayerController, 
public IAbilitySystemInterface,
public IBSEquipmentInterface
{
	GENERATED_BODY()

public:
	
	explicit ABSPlayerController(const FObjectInitializer& ObjectInitializer);
	
	virtual void SetupInputComponent() override;
	
	virtual void OnUnPossess() override;
	
	virtual void Tick(float DeltaSeconds) override;
	
	virtual void AddPlayerUI();
	
	virtual void InitializeWithAbilitySystem(UCharacterAbilitySystem* InCharacterAbilitySystem);
	
	virtual UBSEquipmentComponent* GetEquipmentComponent() const override;
	
	virtual void ApplyEquipment(const FBSEquipPickupInfo& Pickup) override;

protected:
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<UAbilityBufferComponent> AbilityBufferComponent;
	
	UPROPERTY()
	bool bControlDirectionInput = true;
	
	UPROPERTY()
	FVector2D CachedLookSpeed = {1, 1};
	
	UPROPERTY()
	FTimeline TurnTimeline;
	
	UFUNCTION()
	void QuickTurnTimelineTick(float Alpha);
	
	virtual void BindJumpToAction(UEnhancedInputComponent* EnhancedInputComponent, UInputAction* Action);
	
	//Mainly implemented through IAbilitySystemInterface for UI easy access.
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	UPROPERTY(Transient)
	UCharacterAbilitySystem* CharacterAbilitySystem;
	
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
	UInputAction* BasicAbilityAction;
	
	UPROPERTY(BLueprintReadOnly, EditDefaultsOnly, Category="Input|Actions")
	FGameplayTag BasicAbilityTag;
	
	UPROPERTY(BLueprintReadOnly, EditDefaultsOnly, Category="Input|Actions")
	TMap<FGameplayTag, UInputAction*> AbilityInputMapping; 
	
	UPROPERTY(BLueprintReadOnly, EditDefaultsOnly, Category="Input|Actions")
	UInputAction* SpecialAction;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="UI")
	TSubclassOf<UBSWHud> HudWidgetClass;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="UI")
	TSubclassOf<UBSCharacterPane> CharacterPaneClass;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="UI")
	TSubclassOf<UBSPauseMenu> PauseMenuClass;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="UI")
	TSubclassOf<UBSWDamageNumber> FloatingDamageNumberClass;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="UI")
	TSubclassOf<UBSGameplayRootWidget> GameplayRootWidgetClass;
	
	UPROPERTY()
	UBSGameplayRootWidget* GameplayRootWidgetInstance;
	
public:
	//Can be null if UI hasn't been added or initialized properly
	virtual UCommonActivatableWidgetStack* GetWidgetStack();
	
	virtual UCharacterAbilitySystem* GetCharacterAbilitySystem();
	
	UFUNCTION()
	void OnDamageDealt(UAbilitySystemComponent* InstigatorAsc, UAbilitySystemComponent* TargetAsc, FGameplayTagContainer EffectTags, float BaseDamage, float TotalDamage);
	
	virtual void SpawnFloatingNumber(UAbilitySystemComponent* InstigatorAsc, UAbilitySystemComponent* TargetAsc, FGameplayTagContainer EffectTags, float Number);

	virtual void DestroyPlayerUI();
	
	virtual void SpawnDamageNumber(FGameplayEventData EventData);
	
	virtual void TogglePauseMenu();
	virtual void ToggleCharacterPane();
	virtual bool IsInPauseMenu();
};
