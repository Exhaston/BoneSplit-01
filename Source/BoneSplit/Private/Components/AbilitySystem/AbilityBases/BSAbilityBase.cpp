// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Components/AbilitySystem/AbilityBases/BSAbilityBase.h"

#include "AbilitiesExtensionLib.h"
#include "AbilitySystemComponent.h"
#include "GameplayTask.h"
#include "Abilities/BSExtendedAttributeSet.h"
#include "Actors/Predictables/BSProjectileBase.h"
#include "Camera/CameraComponent.h"
#include "Components/AbilitySystem/BSAbilitySystemComponent.h"

UBSAbilityBase::UBSAbilityBase()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

float UBSAbilityBase::GetCostModifier() const
{
	return 1 - GetAbilitySystemComponentFromActorInfo()->GetNumericAttribute(UBSExtendedAttributeSet::GetAbilityCostModifierAttribute());
}

float UBSAbilityBase::GetCooldownModifier() const
{
	float Cdr = GetAbilitySystemComponentFromActorInfo()->GetNumericAttribute(UBSExtendedAttributeSet::GetCooldownReductionModifierAttribute());
	Cdr = UAbilitiesExtensionLib::AsymptoticDr(Cdr, 1);
	return Cdr;
}

void UBSAbilityBase::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec);
	
	if (FGameplayAbilitySpec* MutableSpec = GetCurrentAbilitySpec(); MutableSpec && InputID > 0)
	{
		MutableSpec->InputID = InputID;
	}
}

void UBSAbilityBase::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                     const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                     const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	if (ActorInfo && ActorInfo->AbilitySystemComponent.IsValid())
	{
		EventHandle = ActorInfo->AbilitySystemComponent.Get()->AddGameplayEventTagContainerDelegate(EventTagsToListenTo, 
		FGameplayEventTagMulticastDelegate::FDelegate::CreateUObject(
		this, &UBSAbilityBase::Native_OnGameplayEventReceived));
	}
	else
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}
}

TSoftObjectPtr<UTexture2D> UBSAbilityBase::GetIcon_Implementation() const
{
	return AbilityIcon;
}

void UBSAbilityBase::Native_OnGameplayEventReceived(const FGameplayTag EventTag, const FGameplayEventData* Payload)
{
	BP_OnGameplayEventReceived(EventTag, *Payload);
}

void UBSAbilityBase::BP_OnGameplayEventReceived_Implementation(FGameplayTag EventTag, const FGameplayEventData Payload)
{
}

void UBSAbilityBase::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const bool bReplicateEndAbility, const bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	
	if (EventHandle.IsValid() && ActorInfo && ActorInfo->AbilitySystemComponent.IsValid())
	{
		ActorInfo->AbilitySystemComponent.Get()->RemoveGameplayEventTagContainerDelegate(
			EventTagsToListenTo, EventHandle);
		
		EventHandle.Reset();
	}
}

void UBSAbilityBase::SpawnProjectileForPlayer(TSubclassOf<ABSProjectileBase> Projectile, FTransform SpawnTransform)
{
	if (!IsLocallyControlled() || !IsActive()) return;
	if (const UCameraComponent* CameraComponent = GetAvatarActorFromActorInfo()->GetComponentByClass<UCameraComponent>())
	{
		
		/*
		GetBSAbilitySystemComponent()->Server_SpawnProjectile(
			GetAvatarActorFromActorInfo(),
			Projectile,                           
			SpawnTransform, 
			CameraComponent->GetComponentTransform());
		
		ABSProjectileBase::SpawnProjectileCameraAdjusted(
			GetAvatarActorFromActorInfo(), 
			Projectile,
			SpawnTransform, 
			CameraComponent->GetComponentTransform());
			*/
	}
}

void UBSAbilityBase::SpawnProjectileForMob(const TSubclassOf<ABSProjectileBase> Projectile, const FTransform SpawnTransform, const int32 NumProjectiles, float ConeAngle, const bool bScaleWithMultiHit)
{
	/*
	if (GetBSAbilitySystemComponent()->IsOwnerActorAuthoritative() && NumProjectiles > 0 && Projectile)
	{
		GetBSAbilitySystemComponent()->NetMulticast_SpawnProjectileForMob(GetAvatarActorFromActorInfo(), Projectile, SpawnTransform, NumProjectiles, ConeAngle, bScaleWithMultiHit);
	}

*/
}

void UBSAbilityBase::GetTargetRotationForProjectile(const FVector& ProjectileOrigin, FVector& BaseAimDirection, FVector& OutProjectileBaseAimPoint, FVector& OutCameraBaseAimPoint) const
{
	FVector RayOrigin = FVector::ZeroVector;
	FRotator RayRotation = FRotator::ZeroRotator;
	
	APlayerController* PlayerController = nullptr;
	
	if (GetCurrentActorInfo()->PlayerController.IsValid())
	{
		PlayerController = GetCurrentActorInfo()->PlayerController.Get();
	}
	
	if (PlayerController)
	{
		PlayerController->GetPlayerViewPoint(RayOrigin, RayRotation);
	}
	else
	{
		GetCurrentActorInfo()->AvatarActor->GetActorEyesViewPoint(RayOrigin, RayRotation);
	}
	
	FVector TraceStart = RayOrigin + RayRotation.Vector() * FVector::Distance(ProjectileOrigin, RayOrigin);
	const FVector TraceEnd = RayOrigin + RayRotation.Vector() * FLT_MAX;

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetCurrentActorInfo()->AvatarActor.Get());
	
	FCollisionObjectQueryParams ObjectParams;
	ObjectParams.AddObjectTypesToQuery(ECC_Pawn);
	ObjectParams.AddObjectTypesToQuery(ECC_WorldStatic);
	
	GetWorld()->LineTraceSingleByObjectType(
		Hit,
		TraceStart,
		TraceEnd,
		ObjectParams,
		Params
	);
	
	const FVector AimPoint = Hit.bBlockingHit ? Hit.ImpactPoint : TraceEnd;
	OutProjectileBaseAimPoint = AimPoint;
	OutCameraBaseAimPoint = TraceEnd;
	BaseAimDirection = RayRotation.Vector();
}

UBSAbilitySystemComponent* UBSAbilityBase::GetBSAbilitySystemComponent() const
{
	return Cast<UBSAbilitySystemComponent>(GetAbilitySystemComponentFromActorInfo());
}


