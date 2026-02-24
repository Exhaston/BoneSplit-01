// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "BSAbilitySystemComponent.generated.h"

class ABSProjectileBase;

DECLARE_MULTICAST_DELEGATE_OneParam(FBSOnAbilityGranted, FGameplayAbilitySpec& AbilitySpec);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), DisplayName="BSAbiltiySystemComponent")
class BONESPLIT_API UBSAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	
	UBSAbilitySystemComponent();
	
	virtual void InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor) override;
	
	FSimpleMulticastDelegate OnReplicated;
	
#pragma region AnimationAdjustments
	
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
	
#pragma endregion
	
#pragma region Abilities
	
	// =================================================================================================================
	// Ability Helpers
	// ================================================================================================================= 
	
	FBSOnAbilityGranted OnAbilityGrantedDelegate;
	
	virtual void OnGiveAbility(FGameplayAbilitySpec& AbilitySpec) override;
	void NotifyAbilitiesTo(FBSOnAbilityGranted::FDelegate&& Callback);
	virtual void OnRemoveAbility(FGameplayAbilitySpec& AbilitySpec) override;
	
	UPROPERTY()
	TArray<FGameplayAbilitySpecHandle> GrantedAbilities;
	
	virtual bool CancelAbilitiesWithTag(FGameplayTag InTag);
	
#pragma endregion
	
#pragma region Spawning
	
	// =================================================================================================================
	// Spawning Non Replicated Actors for all clients
	// ================================================================================================================= 
	
	UFUNCTION(Server, Reliable)
	void Server_SpawnActor(AActor* Owner, TSubclassOf<AActor> ActorToSpawn, FTransform SpawnTransform);

	UFUNCTION(Server, Reliable)
	void Server_SpawnProjectile(AActor* Owner, TSubclassOf<ABSProjectileBase> ClassToSpawn, FTransform SpawnTransform, FTransform CameraTransform);
	
	UFUNCTION(NetMulticast, Reliable)
	void NetMulticast_SpawnProjectileForMob(AActor* Owner, TSubclassOf<ABSProjectileBase> ClassToSpawn, FTransform SpawnTransform, int32 NumProjectiles = 1, float ConeAngle = 15, bool bScaleWithMultiHit = true);

private:
	
	UFUNCTION(NetMulticast, Reliable)
	void NetMulticast_SpawnActor(AActor* Owner, TSubclassOf<AActor> ActorToSpawn, FTransform SpawnTransform);
	
	UFUNCTION(NetMulticast, Reliable)
	void NetMulticast_SpawnProjectile(AActor* Owner, TSubclassOf<ABSProjectileBase> ClassToSpawn, FTransform SpawnTransform, FTransform CameraTransform);
	
#pragma endregion
};
