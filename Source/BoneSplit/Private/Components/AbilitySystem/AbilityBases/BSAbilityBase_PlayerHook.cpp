// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Components/AbilitySystem/AbilityBases/BSAbilityBase_PlayerHook.h"

#include "Abilities/Tasks/AbilityTask_ApplyRootMotionConstantForce.h"
#include "Abilities/Tasks/AbilityTask_Repeat.h"
#include "Actors/InteractableBases/BSHookPoint.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/RootMotionSource.h"
#include "Tasks/AbilityTask_TickTask.h"

bool UBSAbilityBase_PlayerHook::CommitCheck(const FGameplayAbilitySpecHandle Handle,
                                            const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                            FGameplayTagContainer* OptionalRelevantTags)
{

	if (!Super::CommitCheck(Handle, ActorInfo, ActivationInfo, OptionalRelevantTags))
		return false;
	
	if (!ActorInfo->PlayerController.IsValid())
		return false;

	FVector  EyesLoc;
	FRotator EyesRot;
	ActorInfo->PlayerController->GetPlayerViewPoint(EyesLoc, EyesRot);

	const FVector TraceEnd = EyesLoc + EyesRot.Vector() * HookRange;

	FHitResult Hit;
	FCollisionObjectQueryParams ObjParams;
	ObjParams.AddObjectTypesToQuery(ECC_WorldDynamic);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(ActorInfo->AvatarActor.Get());

	GetWorld()->LineTraceSingleByObjectType(Hit, EyesLoc, TraceEnd, ObjParams, QueryParams);
	
	CachedHookPoint = Cast<ABSHookPoint>(Hit.GetActor());

	return CachedHookPoint.IsValid();
}

void UBSAbilityBase_PlayerHook::ActivateAbility(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	const ACharacter* OwnerCharacter = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	
    if (!OwnerCharacter || !CommitCheck(Handle, ActorInfo, ActivationInfo) || 
    	!CachedHookPoint.IsValid()  || !CommitAbility(Handle, ActorInfo, ActivationInfo))
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }
	
	OwnerCharacter->GetCharacterMovement()->SetMovementMode(MOVE_Falling);
	
	const float MaxSpeed = OwnerCharacter->GetCharacterMovement()->GetMaxSpeed();
	
	const FVector Direction = CachedHookPoint->GetTargetLocation() - OwnerCharacter->GetActorLocation();
	
	const float Distance = Direction.Size();
	
	const float RequiredDuration = Distance / MoveSpeed;

	// Root Motion Constant Force task — pushes the character at a fixed velocity
	// using root motion (bypasses normal movement input, respects NavMesh/CMC)
	UAbilityTask_ApplyRootMotionConstantForce* RootMotionTask =
		UAbilityTask_ApplyRootMotionConstantForce::ApplyRootMotionConstantForce(
			this,
			FName("HookConstantForce"),
			Direction.GetSafeNormal(),
			MoveSpeed,
			RequiredDuration,
			false,
			nullptr,
			ERootMotionFinishVelocityMode::SetVelocity,
			Direction.GetSafeNormal() * MoveSpeed * 0.35f,
			MoveSpeed * 0.25f,
			false
		);
	
	RootMotionTask->OnFinish.AddDynamic(
		this, &UBSAbilityBase_PlayerHook::OnMoveToHookCompleted);
	
	UAbilityTask_TickTask* TickTask = UAbilityTask_TickTask::AbilityTaskOnTick(this, FName("HookTickTask"));
	TickTask->OnTick.AddDynamic(this, &UBSAbilityBase_PlayerHook::OnHookTick);

	RootMotionTask->ReadyForActivation();
	TickTask->ReadyForActivation();
}

void UBSAbilityBase_PlayerHook::OnHookTick(float DeltaSeconds)
{
	//Specifically to force the character to fall every frame. 
	//Issue being sliding downhill if root motion is applied on slopes.
	const ACharacter* OwnerCharacter = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (!OwnerCharacter) return;
	OwnerCharacter->GetCharacterMovement()->SetMovementMode(MOVE_Falling);
}

void UBSAbilityBase_PlayerHook::OnMoveToHookCompleted()
{
    const FGameplayAbilitySpecHandle  Handle     = GetCurrentAbilitySpecHandle();
    const FGameplayAbilityActorInfo*  ActorInfo  = GetCurrentActorInfo();
    const FGameplayAbilityActivationInfo ActivInfo = GetCurrentActivationInfo();
	
	CachedHookPoint = nullptr;

    EndAbility(Handle, ActorInfo, ActivInfo, true, false);
}
