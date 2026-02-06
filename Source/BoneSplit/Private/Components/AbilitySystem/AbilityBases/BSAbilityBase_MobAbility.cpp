// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Components/AbilitySystem/AbilityBases/BSAbilityBase_MobAbility.h"

#include "AIController.h"
#include "Actors/Mob/BSMobCharacter.h"
#include "Actors/Mob/BSMobSpawner.h"
#include "Actors/Mob/BSTargetSetting.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Components/AbilitySystem/AbilityTasks/BSPlayMontageWaitForEvent.h"
#include "GameFramework/CharacterMovementComponent.h"

UBSAbilityBase_MobAbility::UBSAbilityBase_MobAbility()
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
}

bool UBSAbilityBase_MobAbility::CommitCheck(const FGameplayAbilitySpecHandle Handle,
                                            const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                            FGameplayTagContainer* OptionalRelevantTags)
{
	bool ValidTarget = true; 
	
	if (!ActorInfo->AvatarActor.IsValid()) return false;
	
	/*
	if (UBSMobManager* MobManager = ActorInfo->AvatarActor.Get()->GetWorld()->GetSubsystem<UBSMobManager>())
	{
		if (!MobManager->TryClaimToken(ActorInfo->AvatarActor.Get()) && bRequireToken) return false;
	}
	*/
	//If no target mode we want to activate regardless for custom BP logic, as multi targets etc.
	
	if (bRequireGrounded)
	{
		if (const ACharacter* AvatarCharacter = Cast<ACharacter>(ActorInfo->AvatarActor.Get()))
		{
			if (!AvatarCharacter->GetCharacterMovement()->IsMovingOnGround()) return false;
		}
	}
	
	if (TargetSetting)
	{
		CurrentTarget = TargetSetting->GetTarget(ActorInfo->AvatarActor.Get());
		ValidTarget = CurrentTarget.IsValid();
	}
	
	bool CommitSuccess = ValidTarget && Super::CommitCheck(Handle, ActorInfo, ActivationInfo, OptionalRelevantTags);
	
	if (CommitSuccess)
	{
		AAIController* AIcontroller = UAIBlueprintHelperLibrary::GetAIController(GetAvatarActorFromActorInfo()->GetInstigatorController());
		AIcontroller->SetFocus(CurrentTarget.Get(), EAIFocusPriority::Gameplay);
	}

	return CommitSuccess;
}

void UBSAbilityBase_MobAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	if (CurrentTarget.IsValid() && bAutoMoveToTarget)
	{
		if (ABSMobCharacter* MobCharacter = Cast<ABSMobCharacter>(GetAvatarActorFromActorInfo()))
		{
			//MobCharacter->DesiredStoppingDistance = StoppingDistance;
		}
		
		if (AAIController* AiController = UAIBlueprintHelperLibrary::GetAIController(GetAvatarActorFromActorInfo()))
		{
			AiController->SetFocus(CurrentTarget.Get(), EAIFocusPriority::Gameplay);
		}
	}
	
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UBSAbilityBase_MobAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const bool bReplicateEndAbility, const bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	
	if (HasAuthority(&ActivationInfo) && bAutoReleaseToken)
	{
		UBSMobManager* MobManager = GetAvatarActorFromActorInfo()->GetWorld()->GetSubsystem<UBSMobManager>();
		MobManager->ReleaseToken(GetAvatarActorFromActorInfo());
	}
}

AActor* UBSAbilityBase_MobAbility::GetTarget() const
{
	return CurrentTarget.IsValid() ? CurrentTarget.Get() : nullptr;
}

void UBSAbilityBase_MobAbility::StartMontageTask(
	UAnimMontage* MontageAsset, const FGameplayTagContainer EventTags, const float Rate, const float RootMotionScale)
{
	
	UBSAT_PlayMontageAndWaitForEvent* MontageTask = UBSAT_PlayMontageAndWaitForEvent::PlayMontageAndWaitForEvent(
		this, 
		"MontageTask",
		MontageAsset,
		EventTags,
		Rate,
		true,
		NAME_None,
		true,
		RootMotionScale);
	
	MontageTask->OnCompleted.AddDynamic(this, &UBSAbilityBase_MobAbility::OnMontageFinished);
	MontageTask->OnInterrupted.AddDynamic(this, &UBSAbilityBase_MobAbility::OnMontageFinished);
	MontageTask->EventReceived.AddDynamic(this, &UBSAbilityBase_MobAbility::OnMontageEventReceived);
	
	MontageTask->ReadyForActivation();
}

void UBSAbilityBase_MobAbility::OnMontageFinished(FGameplayTag Tag, FGameplayEventData Payload)
{
	EndAbility(
		GetCurrentAbilitySpecHandle(), 
		GetCurrentActorInfo(), 
		GetCurrentActivationInfo(), 
		true, 
		false);
}

void UBSAbilityBase_MobAbility::OnMontageEventReceived(FGameplayTag Tag, FGameplayEventData Payload)
{
	MontageEventReceived(Tag, Payload);
}

void UBSAbilityBase_MobAbility::MontageEventReceived(FGameplayTag, FGameplayEventData Payload)
{
}
