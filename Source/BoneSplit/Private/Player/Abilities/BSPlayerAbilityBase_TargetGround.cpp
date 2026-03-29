// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Player/Abilities/BSPlayerAbilityBase_TargetGround.h"

#include "Projectiles/BSProjectileSpawnerComponent.h"
#include "Tasks/AbilityTask_TickTask.h"

void UBSPlayerAbilityBase_TargetGround::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                        const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                                        const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	if (CommitAbility(Handle, ActorInfo, ActivationInfo) && ActorToSpawn)
	{
		
		if (IsLocallyControlled())
		{
			UAbilityTask_TickTask* NewTickTask = UAbilityTask_TickTask::AbilityTaskOnTick(this, FName("AbilityTickTask"));
		
			NewTickTask->OnTick.AddDynamic(this, &UBSPlayerAbilityBase_TargetGround::OnAbilityTick);
		
			NewTickTask->ReadyForActivation();
		
			TargetingActor = GetWorld()->SpawnActor(TargetActorClass);
		}
		return;
	}
	
	CancelAbility(Handle, ActorInfo, ActivationInfo, true);
}

void UBSPlayerAbilityBase_TargetGround::OnAbilityTick(float DeltaSeconds)
{
	if (!IsLocallyControlled()) return;
	
	const APlayerController* PC = 
		GetCurrentActorInfo()->PlayerController.IsValid() ? GetCurrentActorInfo()->PlayerController.Get() : nullptr;
	
	if (PC && TargetingActor)
	{
		FVector EyeLocation;
		FRotator EyeRotation;
		
		PC->GetPlayerViewPoint(EyeLocation, EyeRotation);
		
		FCollisionObjectQueryParams CollParams;
		CollParams.AddObjectTypesToQuery(ECC_WorldStatic);
		
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(GetAvatarActorFromActorInfo());
		QueryParams.bTraceComplex = true;

		if (FHitResult HitResult; 
			GetWorld()->LineTraceSingleByObjectType(
				HitResult, 
				EyeLocation, 
				EyeLocation + EyeRotation.Vector() * FLT_MAX, 
				CollParams, QueryParams))
		{
			//FVector TargetUp = HitResult.ImpactNormal;
			TargetingActor->SetActorLocation(HitResult.ImpactPoint);
			
			FVector PCForward = FRotator(0, PC->GetControlRotation().Yaw, 0).Vector();
			FRotator TargetRotation = FRotationMatrix::MakeFromZX(HitResult.ImpactNormal, PCForward).Rotator();
			TargetingActor->SetActorRotation(TargetRotation);
		}
	}
}

void UBSPlayerAbilityBase_TargetGround::InputReleased(const FGameplayAbilitySpecHandle Handle,
                                           const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);
	
	if (IsActive())
	{
		if (TargetingActor && IsLocallyControlled())
		{
			UBSProjectileSpawnerComponent* ProjectileSpawnerComponent = 
				UBSProjectileSpawnerComponent::GetProjectileComponentFromActor(GetAvatarActorFromActorInfo());
			
			if (ProjectileSpawnerComponent)
			{
				ProjectileSpawnerComponent->SpawnGenericActor(ActorToSpawn, TargetingActor->GetActorTransform());
			}
			TargetingActor->Destroy();
			TargetingActor = nullptr;
		}
	}
	
	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

void UBSPlayerAbilityBase_TargetGround::OnInputReleased_Implementation(FVector TargetPos, FRotator TargetRot)
{
}
