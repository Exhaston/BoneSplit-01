// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "BSGenericMobInterface.h"
#include "GameplayTagContainer.h"
#include "Components/AbilitySystem/BSAbilityLibrary.h"
#include "Components/AbilitySystem/BSAbilitySystemInterface.h"
#include "Components/Targeting/BSThreatComponent.h"
#include "Components/Targeting/BSThreatInterface.h"
#include "GameFramework/Character.h"
#include "BSMobCharacter.generated.h"

class ABSMobController;
class UBSProjectileSpawnerComponent;
class UBSCharacterInitData;
class UCharacterAbilitySystem;
class UNavigationInvokerComponent;
class UBSPatrolComponent;
class UBSAggroComponent;
class UPlayMontageCallbackProxy;
class UAITask_MoveTo;
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
class ABSMobCharacter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBSOnMobDied, ABSMobCharacter*, MobCharacter);

UCLASS(Blueprintable, Abstract, BlueprintType, DisplayName="Mob Character")
class BONESPLIT_API ABSMobCharacter : public ACharacter, public IBSAbilitySystemInterface, public IBSGenericMobInterface, 
public IBSThreatInterface
{
	GENERATED_BODY()

public:
	
	ABSMobController* GetMobController() const;
	
	FBSOnMobDied OnMobDiedDelegate;
	
	UFUNCTION()
	void OnMobDied(UAbilitySystemComponent* InstigatorAsc, UAbilitySystemComponent* TargetAsc, FGameplayTagContainer EffectTags, float BaseDamage, float TotalDamage);
	
	virtual void Destroyed() override;
	// =================================================================================================================
	// Defaults
	// =================================================================================================================
	
	explicit ABSMobCharacter(const FObjectInitializer& ObjectInitializer);
	
	virtual void BeginPlay() override;
	
	virtual void PossessedBy(AController* NewController) override;
	
	virtual void PostInitializeComponents() override;
	
	virtual void Tick(float DeltaSeconds) override;
	
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual FRotator GetBaseAimRotation() const override;
	
	UFUNCTION()
	void OnTargetFound(AActor* InActor);
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	float FollowDistance = 250;
	
	UFUNCTION(BlueprintPure)
	bool GetIsInRangeForAttack() { return FVector::Dist(GetActorLocation(), GetThreatComponent()->GetHighestThreatActor()->GetActorLocation()) <= FollowDistance * 1.25; }
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	bool bInRange = false;
	
	UPROPERTY()
	float AbilityCheckTimeInterval = 1;
	
	UPROPERTY()
	float ElapsedAbilityCheckTime = 0;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	float NameplateCullInterval = 1;
	
	UPROPERTY()
	float ElapsedNameplateCullTime = 0;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TMap<TSubclassOf<UGameplayAbility>, float> AbilityWeightMap;
	
	UFUNCTION(BlueprintNativeEvent)
	void BP_OnNewTarget(AActor* NewTarget);
	
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
	void CommitDeath(UAnimMontage* Montage);
	
	UPROPERTY()
	UAnimMontage* ActiveDeathMontage;
	
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_OnDeath(UAbilitySystemComponent* SourceAsc, float Damage, UAnimMontage* Montage);
	
	UFUNCTION()
	void OnDeathMontageEnded(UAnimMontage* Montage, bool bFinished);
	
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
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	UBSProjectileSpawnerComponent* ProjectileSpawnerComponent;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	UBSCharacterInitData* InitData;
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<UBSPatrolComponent> PatrolComponent;
	
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
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category="BS|Initialization")
	UCharacterAbilitySystem* AbilitySystemComponent;
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category="BS|Initialization")
	UNavigationInvokerComponent* NavigationInvokerComponent;
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	UBSAggroComponent* AggroComponent;
};
