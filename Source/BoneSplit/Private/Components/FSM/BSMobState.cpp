// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Components/FSM/BSMobState.h"

#include "Actors/Mob/BSMobCharacter.h"
#include "AIController.h"
#include "Components/FSM/BSFiniteStateComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

void UBSMobState::NativeEnterState(UBSFiniteStateComponent* InOwnerComponent)
{
	MobCharacter = Cast<ABSMobCharacter>(InOwnerComponent->GetOwner());
	check(MobCharacter.IsValid());
	MobController = MobCharacter.Get()->GetController<AAIController>();
	check(MobController.IsValid());
	
	Super::NativeEnterState(InOwnerComponent);
}

void UBSMobState::NativeEndState(UBSFiniteStateComponent* InOwnerComponent)
{
	Super::NativeEndState(InOwnerComponent);
	if (GetMobCharacter())
	{
		GetMobCharacter()->GetCharacterMovement()->StopActiveMovement();
	}
}

ABSMobCharacter* UBSMobState::GetMobCharacter() const
{
	return MobCharacter.IsValid() ? MobCharacter.Get() : nullptr;
}

AAIController* UBSMobState::GetMobController() const
{
	return MobController.IsValid() ? MobController.Get() : nullptr;
}

UBSThreatComponent* UBSMobState::GetMobThreatComponent() const
{
	return MobCharacter.IsValid() ? MobCharacter.Get()->GetThreatComponent() : nullptr;
}

UAbilitySystemComponent* UBSMobState::GetMobAbilitySystemComponent() const
{
	return MobCharacter.IsValid() ? MobCharacter.Get()->GetAbilitySystemComponent() : nullptr;
}
