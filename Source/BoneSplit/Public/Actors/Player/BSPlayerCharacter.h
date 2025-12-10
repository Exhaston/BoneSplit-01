// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "ClientAuthoritativeCharacter.h"
#include "GameplayTagContainer.h"
#include "Interfaces/BSLaunchableInterface.h"
#include "BSPlayerCharacter.generated.h"

class ABSPlayerStart;
struct FBSEquipmentInstance;
struct FBSSaveData;

class UBSEquipmentMeshComponent;
class UBSAttributeSet;
class UBSInventoryComponent;
class UBSAbilitySystemComponent;

UCLASS()
class BONESPLIT_API ABSPlayerCharacter : public AClientAuthoritativeCharacter, 
public IBSLaunchableInterface, public IAbilitySystemInterface
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
	// Launchable
	// ================================================================================================================= 
	
	virtual void LaunchActor(FVector Direction, float Magnitude) override;
	
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
	
	virtual void InitializeCharacter();
	
	UFUNCTION(Server, Reliable)
	virtual void Server_ReceiveClientSave(const FBSSaveData& SaveData);
	
	UFUNCTION(Client, Reliable)
	virtual void Client_InitComplete();
	
	//Call Launch Actor instead. this is called internally because of Client Authoritative Movement
	UFUNCTION(Client, Reliable)                                                 
	virtual void Client_Launch(FVector NormalizedDirection, float Magnitude);
};
