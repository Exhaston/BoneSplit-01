// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "BSAbilitySystemComponent.generated.h"


struct FBSProjectileAlignment;
class ABSProjectileBase;
class ABSPredictableActor;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class BONESPLIT_API UBSAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	
	UBSAbilitySystemComponent();
	
	// =================================================================================================================
	// Overrides for animation speed / blend time scaling
	// ================================================================================================================= 
	
	static void AdjustBlendTimeForMontage(
		const UAnimInstance* TargetAnimInstance, const UAnimMontage* Montage, float InRate);

	virtual float PlayMontage(
		UGameplayAbility* AnimatingAbility, 
		FGameplayAbilityActivationInfo ActivationInfo, 
		UAnimMontage* Montage, 
		float InPlayRate, 
		FName StartSectionName = NAME_None, 
		float StartTimeSeconds = 0.0f) override;
	
	virtual float PlayMontageSimulated(
		UAnimMontage* Montage, float InPlayRate, FName StartSectionName = NAME_None) override;
	
	virtual bool CancelAbilitiesWithTag(FGameplayTag InTag);

	UFUNCTION(Server, Reliable)
	void Server_SpawnProjectile(AActor* Owner, TSubclassOf<ABSProjectileBase> ClassToSpawn, FTransform SpawnTransform, FTransform CameraTransform);

private:
	
	UFUNCTION(NetMulticast, Reliable)
	void NetMulticast_SpawnProjectile(AActor* Owner, TSubclassOf<ABSProjectileBase> ClassToSpawn, FTransform SpawnTransform, FTransform CameraTransform);
};
