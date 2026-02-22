// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "BSSaveGame.h"
#include "Components/Inventory/BSInventoryComponent.h"
#include "GameFramework/PlayerState.h"
#include "BSPlayerState.generated.h"

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
class BONESPLIT_API ABSPlayerState : public APlayerState, public IAbilitySystemInterface, public IBSInventoryInterface
{
	GENERATED_BODY()
	
public:
	
	explicit ABSPlayerState(const FObjectInitializer& ObjectInitializer);
	
	virtual void PostInitializeComponents() override;
	
	virtual void BeginPlay() override;
	
	virtual void SeamlessTravelTo(APlayerState* NewPlayerState) override;
	
	virtual void CopyProperties(APlayerState* PlayerState) override;
	
	virtual void OnDeactivated() override;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	//Send the save data to the server to initialize the Asc.
	UFUNCTION(Server, Reliable)
	void Server_ReceiveSaveData(const FBSSaveData& SaveData);
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	UFUNCTION(BlueprintPure)
	UBSTalentComponent* GetTalentComponent() const;
	
	//Checks if the server received save data and has set up Asc.
	bool GetIsInitialized() const;
	
	UBSAbilitySystemComponent* GetBSAbilitySystem() const;
	
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
	
	FBSOnPlayerState OnPlayerStateReadyDelegate;
	
	virtual UBSInventoryComponent* GetInventoryComponent() override;
	
protected:
	
	void OnDamageOther(FGameplayTag EventTag, const FGameplayEventData* Payload);
	
	UFUNCTION(Client, Reliable)
	void Client_SpawnDamageNumber(FGameplayEventData Payload);
	
	UFUNCTION()
	void OnSaveLoaded(UBSSaveGame* SaveGame);
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_Initialized)
	bool bInitialized = false;
	
	UFUNCTION()
	void OnRep_Initialized() const;
	
	UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly)
	TObjectPtr<UBSTalentComponent> TalentComponent;
	
	UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly)
	TObjectPtr<UBSAbilitySystemComponent> AbilitySystemComponent;
	
	UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly)
	TObjectPtr<UBSInventoryComponent> InventoryComponent;
	
	UPROPERTY()
	TArray<FBSEquipmentDropInfo> LootSpawnInfo;
};
