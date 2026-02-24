// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Components/AbilitySystem/BSAbilitySystemComponent.h"
#include "Actors/Predictables/BSProjectileBase.h"
#include "Components/AbilitySystem/BSAttributeSet.h"
#include "Components/Inventory/BSEquipment.h"


UBSAbilitySystemComponent::UBSAbilitySystemComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	

}

void UBSAbilitySystemComponent::InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor)
{
	Super::InitAbilityActorInfo(InOwnerActor, InAvatarActor);
	
	OnReplicated.Broadcast();
}

#pragma region AnimationAdjustments
	
// =================================================================================================================
// Overrides for animation speed / blend time scaling
// ================================================================================================================= 

void UBSAbilitySystemComponent::AdjustBlendTimeForMontage(
	const UAnimInstance* TargetAnimInstance, const UAnimMontage* Montage, const float InRate)
{
	check(TargetAnimInstance);
	check(Montage);
	if (FMath::IsNearlyZero(InRate)) return;
	FAnimMontageInstance* MontageInstance = TargetAnimInstance->GetInstanceForMontage(Montage);
	MontageInstance->DefaultBlendTimeMultiplier = 1 / InRate;
}

float UBSAbilitySystemComponent::PlayMontage(
	UGameplayAbility* AnimatingAbility,
	const FGameplayAbilityActivationInfo ActivationInfo, 
	UAnimMontage* Montage,
	const float InPlayRate,
	const FName StartSectionName,
	const float StartTimeSeconds)
{
	check(Montage);
	
	const float AnimationLength = Super::PlayMontage(
		AnimatingAbility, ActivationInfo, Montage, InPlayRate, StartSectionName, StartTimeSeconds);
	
	AdjustBlendTimeForMontage(AbilityActorInfo->GetAnimInstance(), Montage, InPlayRate);
	return AnimationLength;
}

float UBSAbilitySystemComponent::PlayMontageSimulated(
	UAnimMontage* Montage, const float InPlayRate, const FName StartSectionName)
{
	const float AnimationLength = Super::PlayMontageSimulated(Montage, InPlayRate, StartSectionName);
	
	AdjustBlendTimeForMontage(AbilityActorInfo->GetAnimInstance(), Montage, InPlayRate);
	return AnimationLength;
}

void UBSAbilitySystemComponent::OnGiveAbility(FGameplayAbilitySpec& AbilitySpec)
{
	Super::OnGiveAbility(AbilitySpec);
	GrantedAbilities.Add(AbilitySpec.Handle);
	OnAbilityGrantedDelegate.Broadcast(AbilitySpec);
}

void UBSAbilitySystemComponent::NotifyAbilitiesTo(FBSOnAbilityGranted::FDelegate&& Callback)
{
	for (const FGameplayAbilitySpecHandle& Handle : GrantedAbilities)
	{
		if (FGameplayAbilitySpec* Spec = FindAbilitySpecFromHandle(Handle))
		{
			Callback.Execute(*Spec);
		}
	}
	OnAbilityGrantedDelegate.Add(MoveTemp(Callback));
}

void UBSAbilitySystemComponent::OnRemoveAbility(FGameplayAbilitySpec& AbilitySpec)
{
	Super::OnRemoveAbility(AbilitySpec);
	GrantedAbilities.Remove(AbilitySpec.Handle);
}

#pragma endregion

#pragma region Abilities
	
// =================================================================================================================
// Ability Helpers
// ================================================================================================================= 

bool UBSAbilitySystemComponent::CancelAbilitiesWithTag(const FGameplayTag InTag)
{
	bool Result = false;
	
	TArray<FGameplayAbilitySpec*> FoundSpecs;
	GetActivatableGameplayAbilitySpecsByAllMatchingTags(
		FGameplayTagContainer(InTag), FoundSpecs);

	for (const auto FoundSpec : FoundSpecs)
	{
		if (FoundSpec->GetPrimaryInstance()->IsActive())
		{
			Result = true;
			CancelAbilityHandle(FoundSpec->Handle);
		}
	}
	
	return Result;
}

void UBSAbilitySystemComponent::NetMulticast_SpawnProjectileForMob_Implementation(AActor* Owner, const TSubclassOf<ABSProjectileBase> ClassToSpawn, const FTransform SpawnTransform, const int32 NumProjectiles, float ConeAngle, const bool bScaleWithMultiHit)
{
	ABSProjectileBase::SpawnProjectiles(Owner, ClassToSpawn, SpawnTransform, NumProjectiles + (bScaleWithMultiHit ? GetNumericAttribute(UBSAttributeSet::GetMultiHitAttribute()) : 0), ConeAngle);
}

#pragma endregion

#pragma region Spawning
	
// =================================================================================================================
// Spawning Non Replicated Actors for all clients
// ================================================================================================================= 

void UBSAbilitySystemComponent::Server_SpawnActor_Implementation(AActor* Owner, const TSubclassOf<AActor> ActorToSpawn,
	const FTransform SpawnTransform)
{
	NetMulticast_SpawnActor(Owner, ActorToSpawn, SpawnTransform);
}

void UBSAbilitySystemComponent::NetMulticast_SpawnActor_Implementation(AActor* Owner, const TSubclassOf<AActor> ActorToSpawn,
                                                                       const FTransform SpawnTransform)
{
	if (AbilityActorInfo.IsValid() && !AbilityActorInfo.Get()->IsLocallyControlled())
	{
		APawn* InstigatorPawn = Cast<APawn>(Owner);
		AActor* SpawnedActor = Owner->GetWorld()->SpawnActorDeferred<AActor>(ActorToSpawn, SpawnTransform, Owner, InstigatorPawn, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		SpawnedActor->FinishSpawning(SpawnTransform);
	}
}

void UBSAbilitySystemComponent::Server_SpawnProjectile_Implementation(AActor* Owner,
                                                                      const TSubclassOf<ABSProjectileBase> ClassToSpawn, const FTransform SpawnTransform, const FTransform CameraTransform)
{
	NetMulticast_SpawnProjectile(Owner, ClassToSpawn, SpawnTransform, CameraTransform);
}

void UBSAbilitySystemComponent::NetMulticast_SpawnProjectile_Implementation(AActor* Owner,
	const TSubclassOf<ABSProjectileBase> ClassToSpawn, const FTransform SpawnTransform, const FTransform CameraTransform)
{
	if (AbilityActorInfo.IsValid() && !AbilityActorInfo.Get()->IsLocallyControlled())
	{
		//ABSProjectileBase::SpawnProjectileCameraAdjusted(Owner, ClassToSpawn, SpawnTransform, CameraTransform);
	}
}

#pragma endregion



