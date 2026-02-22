// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "ClientAuthoritativeCharacter.h"
#include "Components/AbilitySystem/BSAbilitySystemInterface.h"
#include "Components/Inventory/BSEquipment.h"
#include "Components/Inventory/BSEquipmentMeshComponent.h"
#include "Components/Inventory/BSInventoryComponent.h"
#include "BSPlayerCharacter.generated.h"

class UBSInteractionComponent;
class UTextRenderComponent;
struct FGameplayEffectSpec;
class UBSEquipmentEffect;
class UBSEffectMeshComp;
class ABSPlayerState;
class UCameraComponent;
class USpringArmComponent;
class UBSSaveGame;
class UBSLoadingScreenSubsystem;
class ABSPlayerStart;
struct FBSEquipmentInstance;
struct FBSSaveData;

class UBSEquipmentMeshComponent;
class UBSAttributeSet;
class UBSAbilitySystemComponent;

USTRUCT()
struct FBSEquipmentMeshInfo
{
	GENERATED_BODY()
	
	UPROPERTY()
	const UBSEquipmentEffect* SourceEffect;
	
	UPROPERTY()
	TArray<UBSEquipmentMeshComponent*> EquipmentMeshComponents;
	
	bool operator == (const FBSEquipmentMeshInfo& Other) const
	{
		return SourceEffect == Other.SourceEffect;
	}
};

DECLARE_MULTICAST_DELEGATE_TwoParams(FBSOnSkeletalMeshChanged, UBSEquipmentMeshComponent* EquipmentMeshComp, USkeletalMesh* NewAsset);

/**
 * Player Character base with meshes for BSEquipmentEffects and AbilitySystem. Tied to BSPlayerState to function. 
 * The BSPlayerState keeps important data should this character ever get destroyed or removed, 
 * and stand for getting save or initialization data.
 */
UCLASS(DisplayName="Player Character", Blueprintable, BlueprintType, 
	Category="BoneSplit", ClassGroup="BoneSplit", Abstract)
class BONESPLIT_API ABSPlayerCharacter : public AClientAuthoritativeCharacter, 
public IBSAbilitySystemInterface, public IBSInventoryInterface
{
	GENERATED_BODY()
	
public:

#pragma region Defaults
	                                     
	// =================================================================================================================
	// Defaults
	// ================================================================================================================= 
	
	explicit ABSPlayerCharacter(const FObjectInitializer& ObjectInitializer);
	
	virtual void BeginPlay() override;
	
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	virtual void PossessedBy(AController* NewController) override;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual void OnRep_PlayerState() override;
	
#pragma endregion
	
#pragma region Camera
	
	// =================================================================================================================
	// Camera
	// =================================================================================================================
	
	virtual void SetMenuCamera();
	virtual void ResetCamera();
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TObjectPtr<USpringArmComponent> SpringArmComponent;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TObjectPtr<UCameraComponent> CameraComponent;
	
#pragma endregion
	
#pragma region Launching
	
	// =================================================================================================================
	// Launch
	// =================================================================================================================
	
	virtual void Launch(FVector LaunchMagnitude, bool bAdditive) override;
	
	UFUNCTION(Client, Reliable)
	void Client_LaunchCharacter(FVector LaunchVelocity, bool bXYOverride, bool bZOverride);
	
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_OnLaunched(FVector LaunchMagnitude);
	
	virtual void BP_OnLaunched_Implementation(FVector LaunchVelocity) override;
	
#pragma endregion
	
#pragma region Death
	// =================================================================================================================
	// Death
	// =================================================================================================================
	
	UPROPERTY(BlueprintAssignable)
	FBSOnDeathDelegate OnDeathDelegate;
	
	UPROPERTY(ReplicatedUsing=OnRep_Death)
	bool bIsDead;
	
	UFUNCTION()
	void OnRep_Death();
	
	virtual FBSOnDeathDelegate& GetOnDeathDelegate() override;
	
	virtual void Die(UAbilitySystemComponent* SourceAsc, float Damage) override;
	
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_OnDeath(UAbilitySystemComponent* SourceAsc, float Damage);
	
	virtual void BP_OnDeath_Implementation(UAbilitySystemComponent* SourceAsc, float Damage) override;
	
#pragma endregion
	
#pragma region Combat
	
	// =================================================================================================================
	// Combat
	// ================================================================================================================= 
	
	UPROPERTY(BlueprintAssignable)
	FBSOnCombatChangedDelegate OnCombatChangedDelegate;
	
	UPROPERTY(ReplicatedUsing=OnRep_CombatChanged)
	bool bIsInCombat = false;
	
	UFUNCTION()
	void OnRep_CombatChanged();
	
	virtual bool IsInCombat() override;
	
	virtual void BP_OnCombatChanged_Implementation(bool InCombat) override;
	
	virtual bool BP_IsInCombat_Implementation() override;
	
	virtual FBSOnCombatChangedDelegate& GetOnCombatChangedDelegate() override;
	
#pragma endregion
	
#pragma region Asc&Interaction
	
	// =================================================================================================================
	// Interactions & ASC
	// ================================================================================================================= 
	              
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	virtual UBSInventoryComponent* GetInventoryComponent() override;
	
	UFUNCTION(BlueprintPure)
	ABSPlayerState* GetBSPlayerState() const;

protected:
	
	UPROPERTY(meta=(AllowPrivateAccess=true))
	TObjectPtr<UBSInteractionComponent>	InteractionComponent;
	
	UPROPERTY(meta=(AllowPrivateAccess=true))
	TWeakObjectPtr<UBSAbilitySystemComponent> AbilitySystemComponent;
	
#pragma endregion
	
#pragma region Meshes
	
	// =================================================================================================================
	// Meshes
	// ================================================================================================================= 
	
	UFUNCTION()
	void UpdateMeshColors(FColor NewColor);
	
public:
	
	virtual void AddEquipmentMesh(const UBSEquipmentEffect* InSource);
	virtual void RemoveEquipmentMesh(const UBSEquipmentEffect* InSource);
	
	UPROPERTY()
	TArray<FBSEquipmentMeshInfo> EquipmentMeshInfos;
	
	FBSOnSkeletalMeshChanged OnEquipmentMeshChanged;
	
protected:
	
#pragma endregion
	
#pragma region Initialization&Replication
	
	// =================================================================================================================
	// Initialization And Replication
	// ================================================================================================================= 
	
	//Initializes the character from default data and additional save data. Needs to run for all clients and server.
	virtual void InitializeCharacter();
	
	virtual void PostPlayerStateInitialize();
	
	virtual void SetupFloatingName(APlayerState* PS);
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta=(AllowPrivateAccess=true))
	TObjectPtr<UTextRenderComponent> PlayerNameTextComponent;
	
#pragma endregion
};
