// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BSAbilityBase_MobAbility.h"
#include "BSAbilityBase_Teleport.generated.h"

class UBSAT_PlayMontageAndWaitForEvent;
/**
 * 
 */
UCLASS()
class BONESPLIT_API UBSAbilityBase_Teleport : public UBSAbilityBase_MobAbility
{
	GENERATED_BODY()
	
public:
	
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle, 
		const FGameplayAbilityActorInfo* ActorInfo, 
		const FGameplayAbilityActivationInfo ActivationInfo, 
		const FGameplayEventData* TriggerEventData) override;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	UAnimMontage* IntroMontage;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	UAnimMontage* DamageMontage;
	
	//Default Behind
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FVector TargetOffset = {-200,0,0};
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TMap<FGameplayTag, TSubclassOf<UGameplayEffect>> Effects;
	
	UPROPERTY()
	TWeakObjectPtr<AActor> CurrentTargetActor;
	
	UFUNCTION()
	void OnIntroMontageCompleted();
	

};
