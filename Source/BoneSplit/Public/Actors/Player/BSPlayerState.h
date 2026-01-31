// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "BSSaveGame.h"
#include "Components/Inventory/BSEquipment.h"
#include "GameFramework/PlayerState.h"
#include "BSPlayerState.generated.h"

class UBSTalentComponent;
class UBSAttributeSet;
struct FBSSaveData;
class UBSAbilitySystemComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FBSPlayerStateDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBSOnPlayerColorChanged, FColor, NewColor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBSOnEquipmentUpdate, TArray<FBSEquipmentInstance>&, EquipmentArray);

UCLASS(DisplayName="Player State", BlueprintType, Blueprintable, Category="BoneSplit")
class BONESPLIT_API ABSPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:
	
	explicit ABSPlayerState(const FObjectInitializer& ObjectInitializer);
	
	virtual void BeginPlay() override;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	//Send the save data to the server to initialize the Asc.
	UFUNCTION(Server, Reliable)
	void Server_ReceiveSaveData(const FBSSaveData& SaveData);
	
	//Initializes default save data from the BP CDO defined in project settings. 
	//This is a failsafe if any save data fails or new game.
	virtual void InitDefaultSaveData();
	
	UFUNCTION(Client, Reliable)
	void SaveState(bool bSaveToDisk);
	
	//Equips a new equipment instance. Will remove already equipped instance with matching tag.
	UFUNCTION(Server, Reliable)
	void Server_Equip(FBSEquipmentInstance EquipmentInstance);
	
	//Sets the player color and triggers OnPlayerColorChanged Delegate for server and clients.
	UFUNCTION(Server, Reliable)                    
	void Server_SetColor(const int32& NewColor);
	
	//Gets a copy of an equipment instance. See ServerRemoveEquipment or ServerApplyEquipment for altering
	FBSEquipmentInstance FindEquipmentByTag(const FGameplayTag& Slot) const;
	
	int32 FindEquipmentIndexByTag(const FGameplayTag& Slot) const;
	
	//Automatically called from OnRep_EquipmentInstances, remember to run for server
	virtual void NotifyEquipmentUpdated();
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	UFUNCTION(BlueprintPure)
	UBSTalentComponent* GetTalentComponent() const;
	
	//Checks if the server received save data and has set up Asc.
	bool GetIsInitialized() const;
	
	TArray<FBSEquipmentInstance> GetCurrentEquipment() const;
	
	UFUNCTION(BlueprintPure)
	FColor GetPlayerColor() const;
	
	FBSSaveData& GetSaveGame() const;
	
	FBSPlayerStateDelegate& GetInitCompleteDelegate()   { return OnInitComplete; }
	FBSOnEquipmentUpdate& GetEquipmentUpdatedDelegate() { return OnEquipmentUpdate; }
	FBSOnPlayerColorChanged& GetOnPlayerColorChanged()  { return OnPlayerColorChangedDelegate; }
	
protected:
	
	//Equipment instances created from EquipmentCDOs. This holds information that can be altered runtime.
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_EquipmentInstances)
	TArray<FBSEquipmentInstance> EquipmentInstances;
	
	//Called whenever an equipment instance is added or removed.
	UFUNCTION()
	void OnRep_EquipmentInstances();
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_Initialized)
	bool bInitialized = false;
	
	UFUNCTION()
	void OnRep_Initialized() const;
	
	UPROPERTY(ReplicatedUsing=OnRep_PlayerColor)
	int32 PlayerColor = 0;
	
	UFUNCTION()
	void OnRep_PlayerColor() const;
	
	UPROPERTY(BlueprintAssignable)
	FBSPlayerStateDelegate OnInitComplete;

	UPROPERTY(BlueprintAssignable)
	FBSOnEquipmentUpdate OnEquipmentUpdate;
	
	UPROPERTY(BlueprintAssignable)
	FBSOnPlayerColorChanged OnPlayerColorChangedDelegate;
	
	UPROPERTY()
	TObjectPtr<UBSTalentComponent> TalentComponent;
	
	UPROPERTY()
	TObjectPtr<UBSAbilitySystemComponent> AbilitySystemComponent;
	
	UPROPERTY()
	TObjectPtr<UBSAttributeSet> AttributeSetSubObject;
	
private:
	//Restores effects from the save data struct. It will keep effects that had a duration or infinite
	virtual void RestoreEffectsFromSave(const FBSSaveData& SaveData);
	
	//Restores any tags from the save, then removes the tags that is contained in default save object to avoid duplicates.
	virtual void RestoreTagsFromSave(const FBSSaveData& SaveData);
	
	//Restores all the equipment from the save file and equips new instances of them. 
	//These will be fresh instances so apply state back to them if needed.
	virtual void RestoreEquipmentFromSave(const FBSSaveData& SaveData);
	
	//Restores attribute bases from the save struct. An attribute base is the value before any state is applied, 
	//like temporary effects.
	virtual void RestoreAttributesFromSave(const FBSSaveData& SaveData);
	
	virtual void RestoreTalentsFromSave(const FBSSaveData& SaveData);

	void SaveGameplayEffects(FBSSaveData& SaveData, UBSSaveGame* SaveGame);
	void SaveGameplayTags(FBSSaveData& SaveData, UBSSaveGame* SaveGame);
	void SaveEquipment(FBSSaveData& SaveData);
	void SaveAttributes(FBSSaveData& SaveData);
	void SaveTalents(FBSSaveData& SaveData) const;
	
	void Internal_ApplyEquipment(const FBSEquipmentInstance& ItemInstance);
	void Internal_RemoveEquipment(int32 Index);
};
