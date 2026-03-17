// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "BSSaveGame.h"
#include "Components/Inventory/BSInventoryComponent.h"
#include "Equipment/BSEquipmentInterface.h"
#include "GameFramework/PlayerState.h"
#include "Widgets/HUD/BSWHud.h"
#include "BSPlayerState.generated.h"

class UBSEquipmentComponent;
class UCharacterAbilitySystem;
class UBSInventoryComponent;
class ABSPlayerState;
struct FBSEquipmentDropInfo;
class UBSTalentComponent;
class UBSAttributeSet;
struct FBSSaveData;
class UBSAbilitySystemComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FBSPlayerStateDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBSOnPlayerColorChangedDep, FColor, NewColor);

DECLARE_MULTICAST_DELEGATE(FBSOnPlayerState);

UCLASS(DisplayName="Player State", Abstract, BlueprintType, Blueprintable, Category="BoneSplit")
class BONESPLIT_API ABSPlayerState : public APlayerState, 
public IAbilitySystemInterface, 
public IBSInventoryInterface,
public IBSEquipmentInterface
{
	GENERATED_BODY()
	
public:
	
	explicit ABSPlayerState(const FObjectInitializer& ObjectInitializer);
	
	virtual UBSEquipmentComponent* GetEquipmentComponent() const override { return EquipmentComponent; }
	
	virtual void ApplyEquipment(const FBSEquipPickupInfo& Pickup) override;
	
	UCharacterAbilitySystem* GetCharacterAbilitySystem() const;
	
	virtual void PostInitializeComponents() override;
	
	virtual void BeginPlay() override;
	
	virtual void ClientInitialize(AController* C) override;
	
	virtual void CopyProperties(APlayerState* PlayerState) override;
	
	//Send the save data to the server to initialize the Asc.
	UFUNCTION(Server, Reliable)
	void Server_ReceiveSaveData(const FBSSaveData& SaveData);
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	UFUNCTION(BlueprintPure)
	UBSTalentComponent* GetTalentComponent() const;
	
	//Checks if the server received save data and has set up Asc.
	bool GetHasAscData() const;
	
	UFUNCTION(Server, Reliable)
	void Server_ReceiveWantPause();
	
	UFUNCTION(Server, Reliable)
	void Server_ReceiveWantResume();
	
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	UFUNCTION()
	void OnPlayerPaused();
	
	UFUNCTION()
	void OnPlayerResumed();
	
	UPROPERTY(BlueprintAssignable)
	FBSOnPlayerColorChangedDep OnPlayerColorChangedDelegate;
	
	virtual UBSInventoryComponent* GetInventoryComponent() override;

protected:
	
	UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly)
	TObjectPtr<UBSEquipmentComponent> EquipmentComponent;
	
	void OnDamageOther(FGameplayTag EventTag, const FGameplayEventData* Payload);
	
	UFUNCTION(Client, Reliable)
	void Client_SpawnDamageNumber(FGameplayEventData Payload);
	
	UFUNCTION()
	void OnSaveLoaded(UBSSaveGame* SaveGame);
	
	UPROPERTY()
	bool bIsInitialized = false;
	
	UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly)
	TObjectPtr<UBSTalentComponent> TalentComponent;
	
	UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly)
	TObjectPtr<UCharacterAbilitySystem> AbilitySystemComponent;
	

	
	UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly)
	TObjectPtr<UBSInventoryComponent> InventoryComponent;
	
	UPROPERTY()
	TArray<FBSEquipmentDropInfo> LootSpawnInfo;
};
