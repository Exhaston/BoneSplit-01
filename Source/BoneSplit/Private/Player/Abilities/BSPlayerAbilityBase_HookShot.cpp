// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Player/Abilities/BSPlayerAbilityBase_HookShot.h"

#include "Abilities/Tasks/AbilityTask_ApplyRootMotionConstantForce.h"
#include "Interactions/BSHookPoint.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Tasks/AbilityTask_TickTask.h"

bool UBSPlayerAbilityBase_HookShot::CommitCheck(const FGameplayAbilitySpecHandle Handle,
                                                const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                                FGameplayTagContainer* OptionalRelevantTags)
{

	if (!Super::CommitCheck(Handle, ActorInfo, ActivationInfo, OptionalRelevantTags))
		return false;
	
	if (!ActorInfo->PlayerController.IsValid())
		return false;
	
	if (!GetWorld()) return false;

	FVector  EyesLoc;
	FRotator EyesRot;
	ActorInfo->PlayerController->GetPlayerViewPoint(EyesLoc, EyesRot);

	const FVector TraceEnd = EyesLoc + EyesRot.Vector() * HookRange;

	FHitResult Hit;
	FCollisionObjectQueryParams ObjParams;
	ObjParams.AddObjectTypesToQuery(ECC_WorldDynamic);
	ObjParams.AddObjectTypesToQuery(ECC_Visibility);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(ActorInfo->AvatarActor.Get());

	bool bHit = GetWorld()->LineTraceSingleByObjectType(Hit, EyesLoc, TraceEnd, ObjParams, QueryParams);
	
	if (!bHit || !Hit.GetActor() || !Hit.GetActor()->IsA(ABSHookPoint::StaticClass())) return false;
	
	CachedHookPoint = Cast<ABSHookPoint>(Hit.GetActor());

	return CachedHookPoint.IsValid();
}

void UBSPlayerAbilityBase_HookShot::ActivateAbility(
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
	
	const FVector Direction = CachedHookPoint->GetTargetLocation() - OwnerCharacter->GetActorLocation();
	
	const float Distance = Direction.Size();
	
	const float RequiredDuration = Distance / MoveSpeed;
	
	UAbilityTask_ApplyRootMotionConstantForce* RootMotionTask =
		UAbilityTask_ApplyRootMotionConstantForce::ApplyRootMotionConstantForce(
			this,
			FName("HookForce"),
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
		this, &UBSPlayerAbilityBase_HookShot::OnMoveToHookCompleted);
	
	UAbilityTask_TickTask* TickTask = UAbilityTask_TickTask::AbilityTaskOnTick(this, FName("HookTickTask"));
	TickTask->OnTick.AddDynamic(this, &UBSPlayerAbilityBase_HookShot::OnHookTick);

	RootMotionTask->ReadyForActivation();
	TickTask->ReadyForActivation();
}

void UBSPlayerAbilityBase_HookShot::OnHookTick(float DeltaSeconds)
{
	//Specifically to force the character to fall every frame. 
	//Issue being sliding downhill if root motion is applied on slopes.
	const ACharacter* OwnerCharacter = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (!OwnerCharacter) return;
	OwnerCharacter->GetCharacterMovement()->SetMovementMode(MOVE_Falling);
}

void UBSPlayerAbilityBase_HookShot::OnMoveToHookCompleted()
{
    const FGameplayAbilitySpecHandle  Handle     = GetCurrentAbilitySpecHandle();
    const FGameplayAbilityActorInfo*  ActorInfo  = GetCurrentActorInfo();
    const FGameplayAbilityActivationInfo ActivInfo = GetCurrentActivationInfo();
	
	CachedHookPoint = nullptr;

    EndAbility(Handle, ActorInfo, ActivInfo, true, false);
}
