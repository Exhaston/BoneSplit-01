// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagAssetInterface.h"
#include "MMobCharacter.h"
#include "Factions/FactionInterface.h"
#include "BSMobCharacterBase.generated.h"

class UBSProjectileSpawnerComponent;
class UMMobAnimInstance;
class UBSMobAbilityChooserComponent;
class UBSUnitPlateComponent;
class ABSMobCharacterBase;
class UBSExtendedAttributeSet;
class UBSCharacterInitData;
class UBSAbilitySystem;

DECLARE_MULTICAST_DELEGATE_OneParam(FBSOnMobDied, ABSMobCharacterBase* MobCharacter);

UCLASS(DisplayName="MobCharacterBase")
class BONESPLIT_API ABSMobCharacterBase : public AMMobCharacter, 
public IAbilitySystemInterface, public IGameplayTagAssetInterface, public IFactionInterface
{
	GENERATED_BODY()

public:
	
	explicit ABSMobCharacterBase(const FObjectInitializer& ObjectInitializer);
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual void PostInitializeComponents() override;
	
	virtual void PossessedBy(AController* NewController) override;
	
	virtual void BeginPlay() override;
	
	virtual FVector GetTargetLocation(AActor* RequestedBy = nullptr) const override;
	
	UFUNCTION(NetMulticast, Reliable)
	void NetMulticast_MobDied();
	
	virtual void AlignRotationToSurface();
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;
	
	virtual bool HasMatchingGameplayTag(FGameplayTag TagToCheck) const override;
	virtual bool HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;
	virtual bool HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;
	
	virtual FGameplayTagContainer& GetFactionTags() override;
	virtual bool HasAnyMatchingFactionTag(FGameplayTagContainer InFactionTags) override;
	
	FBSOnMobDied& GetOnMobDiedDelegate() { return OnMobDiedDelegate; }

protected:
	
	bool bIsDead = false;
	
	virtual UMMobAnimInstance* TryGetMobInstance();
	
	FBSOnMobDied OnMobDiedDelegate;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FText MobName;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TObjectPtr<UBSCharacterInitData> CharacterInitData = nullptr;
	
	UPROPERTY(ReplicatedUsing=OnRep_FactionTags, BlueprintReadOnly, EditAnywhere)
	FGameplayTagContainer FactionTags;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TArray<UAnimMontage*> DeathMontages;
	
	UFUNCTION()
	void OnRep_FactionTags();
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<UBSAbilitySystem> AbilitySystemComponent;
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<UBSExtendedAttributeSet> AttributeSet;
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<UBSMobAbilityChooserComponent> MobAbilityChooserComponent;
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<UBSUnitPlateComponent> UnitPlateComponent;
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<UBSProjectileSpawnerComponent> ProjectileSpawnerComponent;
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<USceneComponent> TargetComponent;
};
