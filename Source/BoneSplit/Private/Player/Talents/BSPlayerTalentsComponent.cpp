// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Player/Talents/BSPlayerTalentsComponent.h"

#include "AbilitySystemComponent.h"
#include "Net/UnrealNetwork.h"


UBSPlayerTalentsComponent::UBSPlayerTalentsComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void UBSPlayerTalentsComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UBSPlayerTalentsComponent, TalentTags);
}

void UBSPlayerTalentsComponent::AddTag(const FGameplayTag InTag)
{
	TalentTags.UpdateTagCount(InTag, 1, EGameplayTagReplicationState::TagAndCountToAll);
}

void UBSPlayerTalentsComponent::RemoveTag(const FGameplayTag InTag)
{
	TalentTags.UpdateTagCount(InTag, -1, EGameplayTagReplicationState::TagAndCountToAll);
}

int32 UBSPlayerTalentsComponent::GetTagCount(const FGameplayTag InTag)
{
	return TalentTags.GetExplicitTagCount(InTag);
}

