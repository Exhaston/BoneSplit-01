// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "BSGenericMobInterface.h"
#include "GameplayTagContainer.h"
#include "Components/AbilitySystem/BSAbilityLibrary.h"
#include "Components/AbilitySystem/BSAbilitySystemInterface.h"
#include "Components/Targeting/BSThreatInterface.h"
#include "GameFramework/Character.h"
#include "BSMobCharacter.generated.h"

class UWidgetComponent;
class UBSFiniteState;
class UBSFiniteStateComponent;
class UBSThreatComponent;
class USphereComponent;
class UBSAttributeSet;
class UGameplayAbility;
class UGameplayEffect;
class UBSAbilitySystemComponent;
class UBSMobMovementComponent;

UCLASS(Blueprintable, Abstract, BlueprintType, DisplayName="Mob Character")
class BONESPLIT_API ABSMobCharacter : public ACharacter, public IBSAbilitySystemInterface, public IBSGenericMobInterface, 
public IBSThreatInterface
{
	GENERATED_BODY()

public:
	
	// =================================================================================================================
	// Defaults
	// =================================================================================================================
	
	explicit ABSMobCharacter(const FObjectInitializer& ObjectInitializer);
	
	virtual void BeginPlay() override;
	
	virtual void Tick(float DeltaSeconds) override;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UPROPERTY(ReplicatedUsing=OnRep_Ready)
	bool bReady = false;
	
	UFUNCTION()
	void OnRep_Ready();
	
	// =================================================================================================================
	// Launching
	// =================================================================================================================
	
	virtual void Launch(FVector LaunchMagnitude, bool bAdditive) override;
	
	virtual void LaunchCharacter(FVector LaunchVelocity, bool bXYOverride, bool bZOverride) override; 
	
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_OnLaunched(FVector LaunchVelocity);
	
	virtual void BP_OnLaunched_Implementation(FVector LaunchVelocity) override;
	
	// =================================================================================================================
	// Death
	// =================================================================================================================   
	
	UPROPERTY(BlueprintAssignable)
	FBSOnDeathDelegate OnDeathDelegate;
	
	virtual FBSOnDeathDelegate& GetOnDeathDelegate() override;
	
	virtual void Die(UAbilitySystemComponent* SourceAsc, float Damage) override;
	
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_OnDeath(UAbilitySystemComponent* SourceAsc, float Damage, UAnimMontage* Montage);
	
	UPROPERTY()
	FTimerHandle DeathTimerHandle;
	
	UPROPERTY(ReplicatedUsing=OnRep_Death)
	bool bIsDead = false;
	
	UFUNCTION()
	void OnRep_Death();
	
	virtual void BP_OnDeath_Implementation(UAbilitySystemComponent* SourceAsc, float Damage) override;
	
	// =================================================================================================================
	// Asc
	// ================================================================================================================= 
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	virtual UBSThreatComponent* GetThreatComponent() override;
	
	virtual bool IsInCombat() override;
	
	virtual bool BP_IsInCombat_Implementation() override;
	
	virtual void BP_OnCombatChanged_Implementation(bool InCombat) override;
	
	UPROPERTY(ReplicatedUsing=OnRep_OnCombatChanged)
	bool bIsInCombat = false;
	
	UFUNCTION()
	void OnRep_OnCombatChanged();
	
	virtual FBSOnCombatChangedDelegate& GetOnCombatChangedDelegate() override;
	
	UPROPERTY(BlueprintAssignable)
	FBSOnCombatChangedDelegate OnCombatChangedDelegate;
	
	UFUNCTION()
	void OnCombatChanged(bool bCombat);
	
	virtual void NativeOnCombatBegin() override;
	virtual void NativeOnCombatEnd() override;
	virtual void NativeOnCombatTick(bool bReceivedToken, float DeltaTime) override;
	

	virtual float BP_GetAggroRange_Implementation() override;

protected:
	
	virtual void SetRandomColor();
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TArray<FLinearColor> ColorVariations;
	
	UPROPERTY(ReplicatedUsing=OnRep_RandomColor)
	int32 RandomColor = -1;
	
	UFUNCTION()
	void OnRep_RandomColor();
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TArray<UAnimMontage*> DeathAnimations;
	
	//Choose death montage according to last damage received before death.
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TMap<FGameplayTag, UAnimMontage*> DeathMontages;
	
	// =================================================================================================================
	// Components
	// =================================================================================================================   
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta=(ClampMin=0, Units=cm))
	float AggroSphereRadius = 1000;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FGameplayTagContainer GrantedTags;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	UBSThreatComponent* ThreatComponent;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="BS|Initialization")
	TArray<TSubclassOf<UGameplayEffect>> Effects;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="BS|Initialization")
	TArray<TSubclassOf<UGameplayAbility>> GameplayAbility;
	
	UPROPERTY()
	TObjectPtr<UBSAttributeSet> AttributeSet;
	
	UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly)
	UWidgetComponent* WidgetComponent;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="BS|Initialization")
	UBSAbilitySystemComponent* AbilitySystemComponent;
};
