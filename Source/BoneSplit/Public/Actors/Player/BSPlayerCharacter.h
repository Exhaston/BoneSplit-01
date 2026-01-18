// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "ClientAuthoritativeCharacter.h"
#include "GameplayTagContainer.h"
#include "Components/AbilitySystem/BSKillableInterface.h"
#include "Interfaces/BSMovementInterface.h"
#include "BSPlayerCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class UBSSaveGame;
class UBSPersistantDataSubsystem;
class ABSPlayerStart;
struct FBSEquipmentInstance;
struct FBSSaveData;

class UBSEquipmentMeshComponent;
class UBSAttributeSet;
class UBSInventoryComponent;
class UBSAbilitySystemComponent;

/**
 * TODO: Document
 * Reasoning for not utilizing the player state as a 
 * hub for information is to simplify branching from network race conditions. 
 * Avoid deleting the player, and if so save data first.
 */
UCLASS(DisplayName="Player Character", Blueprintable, BlueprintType, 
	Category="BoneSplit", ClassGroup="BoneSplit", Abstract)
class BONESPLIT_API ABSPlayerCharacter : public AClientAuthoritativeCharacter, 
public IBSMovementInterface, public IAbilitySystemInterface, public IBSKillableInterface
{
	GENERATED_BODY()

public:
	                                     
	// =================================================================================================================
	// Defaults
	// ================================================================================================================= 
	
	explicit ABSPlayerCharacter(const FObjectInitializer& ObjectInitializer);
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual void PossessedBy(AController* NewController) override;
	
	virtual void OnRep_PlayerState() override;
	
	virtual void Tick(float DeltaSeconds) override;
	
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	// =================================================================================================================
	// Camera
	// ================================================================================================================= 
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TObjectPtr<USpringArmComponent> SpringArmComponent;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TObjectPtr<UCameraComponent> CameraComponent;
	
	// =================================================================================================================
	// Killable
	// ================================================================================================================= 
	
	virtual bool CanBeKilled() const override;
	
	virtual void OnKilled(AActor* Killer, float Damage) override;
	
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_OnKilled(AActor* Killer, float Damage);
	
	// =================================================================================================================
	// Asc
	// ================================================================================================================= 
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

protected:
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta=(AllowPrivateAccess=true))
	TObjectPtr<UBSAbilitySystemComponent> AbilitySystemComponent;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta=(AllowPrivateAccess=true))
	TObjectPtr<UBSAttributeSet> AttributeSet;
	
public:
	
	// =================================================================================================================
	// Movement Interface
	// ================================================================================================================= 
	
	virtual void LaunchActor(FVector Direction, float Magnitude) override;
	
	virtual void SetMovementRotationMode(uint8 NewMovementMode) override;
	
	UPROPERTY()
	bool bAligningToController = false;
	
	// =================================================================================================================
	// Equipment
	// =================================================================================================================
	
	UFUNCTION(Server, Reliable)
	void Server_ApplyEquipment(FBSEquipmentInstance EquipmentInstance);
	
	UFUNCTION(Server, Reliable)
	void Server_ApplyColor(const int32& NewColor);
	
	UPROPERTY(ReplicatedUsing=OnRep_PlayerColor)
	int32 PlayerColor = 0;
	
protected:
	
	
	int32 FindEquipmentIndexByTag(const FGameplayTag& Slot) const;
	
	void UpdateSkeletalMeshes();
	
	UPROPERTY(ReplicatedUsing=OnRep_Equipment)
	TArray<FBSEquipmentInstance> Equipment;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta=(AllowPrivateAccess=true))
	UBSEquipmentMeshComponent* HeadComponent;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta=(AllowPrivateAccess=true))
	UBSEquipmentMeshComponent* ArmsComponent;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta=(AllowPrivateAccess=true))
	UBSEquipmentMeshComponent* LegsComponent;
	
	UFUNCTION()
	void OnRep_Equipment();
	
	UFUNCTION()
	void OnRep_PlayerColor() const;
	
private:
	
	virtual void Internal_RemoveEquipment(int32 Index);
	
	virtual void Internal_ApplyEquipment(const FBSEquipmentInstance& ItemInstance);
	
	// =================================================================================================================
	// Initialization
	// =================================================================================================================
	
protected:
	
	//Initializes the character from default data and additional save data. Needs to run for all clients and server.
	virtual void InitializeCharacter();
	
	//Initializes default save data. This should always be applied before loading any save data as a fallback.
	//Run only on authority.
	virtual void InitializeDefaultData(UBSPersistantDataSubsystem* PersistantSubsystem);
	
	UFUNCTION(Server, Reliable)
	virtual void Server_ReceiveClientSave(const FBSSaveData& SaveData);
	
	virtual void RestoreEffectsFromSave(const FBSSaveData& SaveData);
	virtual void RestoreTagsFromSave(const FBSSaveData& SaveData);
	virtual void RestoreEquipmentFromSave(const FBSSaveData& SaveData);
	virtual void RestoreAttributesFromSave(const FBSSaveData& SaveData);
	
	UFUNCTION(Client, Reliable)
	virtual void Client_InitComplete();
	
	UFUNCTION(BlueprintCallable)
	void BP_SaveState();
	
	//Saves effects, tags, attributes, equipment and color. 
	//bSaveToDisk: optional to save the stored save to persistent storage. 
	//If not it is stored under the game instance lifetime.
	UFUNCTION(Client, Reliable)
	virtual void SaveState(bool bSaveToDisk = true);
	
	virtual void SaveGameplayEffects(FBSSaveData& SaveData, UBSSaveGame* SaveGame);
	virtual void SaveGameplayTags(FBSSaveData& SaveData, UBSSaveGame* SaveGame);
	virtual void SaveEquipment(FBSSaveData& SaveData);
	virtual void SaveAttributes(FBSSaveData& SaveData);
	
	//Call Launch Actor instead. this is called internally because of Client Authoritative Movement
	UFUNCTION(Client, Reliable)                                                 
	virtual void Client_Launch(FVector NormalizedDirection, float Magnitude);
};
