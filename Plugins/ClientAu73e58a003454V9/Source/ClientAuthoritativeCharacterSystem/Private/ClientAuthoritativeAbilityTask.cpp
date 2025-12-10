// Copyright 2023 Anton Hesselbom. All Rights Reserved.

#include "ClientAuthoritativeAbilityTask.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "AbilitySystemComponent.h"
#include "Runtime/Launch/Resources/Version.h"

FClientAuthoritativeRootMotionSource::FClientAuthoritativeRootMotionSource()
{
	// Disable Partial End Tick.
	// Otherwise we end up with very inconsistent velocities on the last frame.
	// This ensures that the ending velocity is maintained and consistent.
	Settings.SetFlag(ERootMotionSourceSettingsFlags::DisablePartialEndTick);
}

void FClientAuthoritativeRootMotionSource::PrepareRootMotion(float SimulationTime, float MovementTickTime, const ACharacter& Character, const UCharacterMovementComponent& MoveComponent)
{
	if (!bIsSetup)
	{
		OnStart(&Character, &MoveComponent);
		bIsSetup = true;
	}

	FVector NewLocation = Update(SimulationTime, &Character, &MoveComponent);
	FVector Velocity = (NewLocation - Character.GetActorLocation()) / SimulationTime;
	FTransform NewTransform(Velocity);
	RootMotionParams.Set(NewTransform);
}

void FClientAuthoritativeRootMotionSource::FinishMovement()
{
	Status.SetFlag(ERootMotionSourceStatusFlags::Finished);
}

void UClientAuthoritativeAbilityTask::Activate()
{
	Super::Activate();

	if (AbilitySystemComponent->AbilityActorInfo->MovementComponent.IsValid())
	{
		MovementComponent = Cast<UCharacterMovementComponent>(AbilitySystemComponent->AbilityActorInfo->MovementComponent.Get());

#if ENGINE_MAJOR_VERSION >= 5 && ENGINE_MINOR_VERSION >= 6
		if (MovementComponent.IsValid())
#else
		if (MovementComponent)
#endif
		{
			ForceName = ForceName.IsNone() ? FName("ClientAuthoritativeAbilityTask") : ForceName;
			TSharedPtr<FClientAuthoritativeRootMotionSource> RootMotionSource = MakeShareable(CreateRootMotionSource());
			RootMotionSource->InstanceName = ForceName;
			RootMotionSource->AccumulateMode = ERootMotionAccumulateMode::Override;
			RootMotionSource->Priority = 5;
			RootMotionSource->FinishVelocityParams.Mode = FinishVelocityMode;
			RootMotionSource->FinishVelocityParams.SetVelocity = FinishSetVelocity;
			RootMotionSource->FinishVelocityParams.ClampVelocity = FinishClampVelocity;

			RootMotionSourceID = MovementComponent->ApplyRootMotionSource(RootMotionSource);

			OnStart();

#if ENGINE_MAJOR_VERSION < 5 || ENGINE_MINOR_VERSION < 3
			if (Ability)
				Ability->SetMovementSyncPoint(ForceName);
#endif
		}
	}
	else
		UE_LOG(LogTemp, Warning, TEXT("UClientAuthoritativeAbilityTask called in Ability %s with null MovementComponent; Task Instance Name %s."), Ability ? *Ability->GetName() : TEXT("NULL"), *InstanceName.ToString());
}

void UClientAuthoritativeAbilityTask::TickTask(float DeltaTime)
{
	if (bIsFinished)
		return;

	Super::TickTask(DeltaTime);

	if (HasTimedOut())
	{
		bIsFinished = true;
		EndTask();
	}
}

void UClientAuthoritativeAbilityTask::OnDestroy(bool AbilityIsEnding)
{
#if ENGINE_MAJOR_VERSION >= 5 && ENGINE_MINOR_VERSION >= 6
	if (MovementComponent.IsValid())
#else
	if (MovementComponent)
#endif
		MovementComponent->RemoveRootMotionSourceByID(RootMotionSourceID);

	OnStop();

	bool ShouldBroadcast = ShouldBroadcastAbilityTaskDelegates();
	Super::OnDestroy(AbilityIsEnding);

	if (ShouldBroadcast)
		OnFinish.Broadcast();
}
