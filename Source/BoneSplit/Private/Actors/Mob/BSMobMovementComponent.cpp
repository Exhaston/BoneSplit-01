// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Actors/Mob/BSMobMovementComponent.h"

#include "AbilitySystemInterface.h"
#include "BoneSplit/BoneSplit.h"
#include "Components/AbilitySystem/BSAttributeSet.h"
#include "Navigation/PathFollowingComponent.h"

UBSMobMovementComponent::UBSMobMovementComponent()
{
	// =================================================================================================================
	// Nav Properties
	// =================================================================================================================   
	
	NavAgentProps.AgentHeight = 144;
	NavAgentProps.AgentRadius = 34;
	NavAgentProps.bCanWalk = true;
	PerchRadiusThreshold = 34;
	PerchAdditionalHeight = 45;
	MaxStepHeight = 45;
	
	SetWalkableFloorAngle(45);
	bSlideAlongNavMeshEdge = true;
	NavWalkingFloorDistTolerance = 0;
	bSweepWhileNavWalking = false;
	
	// =================================================================================================================
	// Character Properties
	// =================================================================================================================   
	
	bUseFlatBaseForFloorChecks = false;
	GravityScale = 2;
	NetworkSmoothingMode = ENetworkSmoothingMode::Linear;
	bOrientRotationToMovement = true;
	DefaultLandMovementMode = MOVE_NavWalking;
	bAllowPhysicsRotationDuringAnimRootMotion = false;
	RotationRate = {0,450, 0};
}

void UBSMobMovementComponent::BeginPlay()
{
	Super::BeginPlay();
	
	// =================================================================================================================
	// Bind and cache values from the ASC
	// =================================================================================================================
	
	const IAbilitySystemInterface* AscInterface = Cast<IAbilitySystemInterface>(GetOwner());
	if (!AscInterface) return;
	
	OwnerAsc = AscInterface->GetAbilitySystemComponent();
	if (!OwnerAsc.IsValid()) return;
		
	UAbilitySystemComponent* Asc = OwnerAsc.Get();
	
	BindAttributes(Asc);
			
	BindTags(Asc);
}

float UBSMobMovementComponent::GetMaxSpeed() const
{
	if (!CanMove()) return 0;
	//1 is default if no Asc has been found. Otherwise, it will be the GetSpeedAttribute().
	return Super::GetMaxSpeed() * CachedSpeedMod;
}

bool UBSMobMovementComponent::CanMove() const
{
	return ImpairmentTags.IsEmpty();
}

void UBSMobMovementComponent::BindTags(UAbilitySystemComponent* InAsc)
{
	for (auto ExistingTag : InAsc->GetOwnedGameplayTags())
	{
		if (BSTags::StopMoveTags.Contains(ExistingTag))
		{
			ImpairmentTags.Add(ExistingTag);
		}
	}
	
	InAsc->RegisterGenericGameplayTagEvent().AddWeakLambda(
	this, [this] (const FGameplayTag Tag, const int32 Count)
	{
		if (BSTags::StopMoveTags.Contains(Tag))
		{
			if (Count > 0)
			{
				ImpairmentTags.Remove(Tag);
			}
			else
			{
				ImpairmentTags.Add(Tag);
			}
		}
	});
}

void UBSMobMovementComponent::BindAttributes(UAbilitySystemComponent* InAsc)
{
	if (SpeedAttribute.IsValid())
	{
		CachedSpeedMod = InAsc->GetNumericAttribute(SpeedAttribute);	
		
		InAsc->GetGameplayAttributeValueChangeDelegate(SpeedAttribute).AddWeakLambda(
		this, [this](const FOnAttributeChangeData& Data)
		{
			CachedSpeedMod = Data.NewValue;
		});
	}
}

