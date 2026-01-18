// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "BSGenericMobInterface.h"
#include "GameplayTagContainer.h"
#include "Components/Targeting/BSThreatInterface.h"
#include "GameFramework/Character.h"
#include "Components/AbilitySystem/BSKillableInterface.h"
#include "Interfaces/BSMovementInterface.h"
#include "BSMobCharacter.generated.h"

class UBSFiniteState;
class UBSFiniteStateComponent;
class UBSThreatComponent;
class USphereComponent;
class UBSAttributeSet;
class UGameplayAbility;
class UGameplayEffect;
class UBSAbilitySystemComponent;
class UBSMobMovementComponent;

UCLASS()
class BONESPLIT_API ABSMobCharacter : public ACharacter, public IAbilitySystemInterface, public IBSGenericMobInterface, 
public IBSMovementInterface, public IBSThreatInterface, public IBSKillableInterface
{
	GENERATED_BODY()

public:
	
	explicit ABSMobCharacter(const FObjectInitializer& ObjectInitializer);
	
	virtual void BeginPlay() override;
	
	virtual void Tick(float DeltaSeconds) override;
	
	virtual void PossessedBy(AController* NewController) override;
	
	virtual void LaunchActor(FVector Direction, float Magnitude) override;
	
	virtual void SetMovementRotationMode(uint8 NewMovementMode) override;
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	virtual void CheckAggroRadius();
	
	virtual UBSThreatComponent* GetThreatComponent() override;
	
	UFUNCTION()
	void OnCombatChanged(bool bCombat);
	
	virtual void OnKilled(AActor* Killer, float Damage) override;
	
	virtual bool CanBeKilled() const override;
	
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_OnKilled(AActor* Killer, float Damage);
	
	virtual void Destroyed() override;
	
	FBSOnKilled OnEnemyKilledDelegate;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float DesiredStoppingDistance = 500;
	
	virtual void NativeOnCombatBegin() override;
	virtual void NativeOnCombatEnd() override;
	virtual void NativeOnCombatTick(bool bReceivedToken, float DeltaTime) override;
	
	virtual bool BP_IsInCombat_Implementation() override;
	virtual float BP_GetAggroRange_Implementation() override;

protected:
	
	UPROPERTY()
	bool bIsInCombat = false;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta=(ClampMin=0, Units=cm))
	float AggroSphereRadius = 1000;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FGameplayTagContainer GrantedTags;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TSubclassOf<UBSFiniteState> IdleState;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TSubclassOf<UBSFiniteState> CombatState;
	
	UPROPERTY()
	FTimerHandle AggroTimerHandle;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	UBSFiniteStateComponent* StateMachineComponent;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	UBSThreatComponent* ThreatComponent;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TArray<TSubclassOf<UGameplayEffect>> Effects;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TArray<TSubclassOf<UGameplayAbility>> GameplayAbility;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TObjectPtr<UBSAttributeSet> AttributeSet;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	UBSAbilitySystemComponent* AbilitySystemComponent;
};
