// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BSMobAbilityChooserComponent.generated.h"


struct FGameplayAbilitySpecHandle;
class UAbilitySystemComponent;
class UBSMobAbility;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class BONESPLIT_API UBSMobAbilityChooserComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	
	UBSMobAbilityChooserComponent();
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
						   FActorComponentTickFunction* ThisTickFunction) override;
	
	virtual FGameplayAbilitySpecHandle EvaluateAndFindAbility();
	
	virtual void StartAbilityChooser(UAbilitySystemComponent* InAbilitySystem);
	virtual void StopAbilityChooser();
	
	virtual void DestroyComponent(bool bPromoteChildren = false) override;

protected:
	
	virtual void OnComponentTickedFromTimer();
	
	FTimerHandle TickTimer;
	
	virtual void BeginPlay() override;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	bool bAutoGrantAbilities = true;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	float TickRate = 1;
	
	UPROPERTY()
	TArray<FGameplayAbilitySpecHandle> GrantedHandles;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta=(ClampMin=0, ClampMax=100))
	TMap<TSubclassOf<UBSMobAbility>, float> AbilityWeightMap;
	
	UPROPERTY()
	TArray<TSubclassOf<UBSMobAbility>> AbilityRecord;
	
	UPROPERTY(Transient)
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

};
