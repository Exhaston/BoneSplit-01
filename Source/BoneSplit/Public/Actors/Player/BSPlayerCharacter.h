// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "ClientAuthoritativeCharacter.h"
#include "Components/AbilitySystem/BSAbilitySystemInterface.h"
#include "BSPlayerCharacter.generated.h"

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
class UBSInventoryComponent;
class UBSAbilitySystemComponent;

/**
 *
 */
UCLASS(DisplayName="Player Character", Blueprintable, BlueprintType, 
	Category="BoneSplit", ClassGroup="BoneSplit", Abstract)
class BONESPLIT_API ABSPlayerCharacter : public AClientAuthoritativeCharacter, 
public IBSAbilitySystemInterface
{
	GENERATED_BODY()

public:
	                                     
	// =================================================================================================================
	// Defaults
	// ================================================================================================================= 
	
	explicit ABSPlayerCharacter(const FObjectInitializer& ObjectInitializer);
	
	virtual void BeginPlay() override;
	
	virtual void PossessedBy(AController* NewController) override;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual void OnRep_PlayerState() override;
	
	// =================================================================================================================
	// Camera
	// ================================================================================================================= 
	
	UFUNCTION()
	void OnUICharacterPane();
	
	UFUNCTION()
	void OnUICharacterPaneClose();
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TObjectPtr<USpringArmComponent> SpringArmComponent;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TObjectPtr<UCameraComponent> CameraComponent;
	
	// =================================================================================================================
	// Launch
	// =================================================================================================================
	
	virtual void Launch(FVector LaunchMagnitude, bool bAdditive) override;
	
	UFUNCTION(Client, Reliable)
	void Client_LaunchCharacter(FVector LaunchVelocity, bool bXYOverride, bool bZOverride);
	
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_OnLaunched(FVector LaunchMagnitude);
	
	virtual void BP_OnLaunched_Implementation(FVector LaunchVelocity) override;
	
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
	
	// =================================================================================================================
	// Asc
	// ================================================================================================================= 
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	UFUNCTION(BlueprintPure)
	ABSPlayerState* GetBSPlayerState() const;

protected:
	
	UPROPERTY(meta=(AllowPrivateAccess=true))
	TWeakObjectPtr<UBSAbilitySystemComponent> AbilitySystemComponent;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta=(AllowPrivateAccess=true))
	TObjectPtr<UTextRenderComponent> PlayerNameTextComponent;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta=(AllowPrivateAccess=true))
	UBSEquipmentMeshComponent* HeadComponent;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta=(AllowPrivateAccess=true))
	UBSEquipmentMeshComponent* ArmsComponent;                  
	
	//Main-hand weapon mesh component. Have a socket in the parent mesh called 'MainHand' and this will snap to it.
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta=(AllowPrivateAccess=true))
	UBSEquipmentMeshComponent* MainHandComponent;
	
	//Off-handed weapon mesh component. Have a socket in the parent mesh called 'OffHand' and this will snap to it.
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta=(AllowPrivateAccess=true))
	UBSEquipmentMeshComponent* OffHandComponent;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta=(AllowPrivateAccess=true))
	UBSEquipmentMeshComponent* LegsComponent;
	
	UFUNCTION()
	void OnPlayerColourChanged(FColor NewColor);
	
	//Initializes the character from default data and additional save data. Needs to run for all clients and server.
	virtual void InitializeCharacter();
	
	virtual void SetupMeshes();
	
	virtual void LoadMeshesFromEquipmentEffect(const UBSEquipmentEffect* EffectMeshComp);
	
	UFUNCTION()
	void OnPlayerStateInitComplete();
	
};
