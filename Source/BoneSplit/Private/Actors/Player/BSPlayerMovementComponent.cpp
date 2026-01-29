// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Actors/Player/BSPlayerMovementComponent.h"

#include "AbilitySystemComponent.h"
#include "BoneSplit/BoneSplit.h"
#include "Components/AbilitySystem/BSAttributeSet.h"
#include "GameFramework/Character.h"

UBSPlayerMovementComponent::UBSPlayerMovementComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	NavAgentProps.AgentHeight = 144;
	NavAgentProps.AgentRadius = 34;
	NavAgentProps.bCanWalk = true;
	PerchRadiusThreshold = 10;
	PerchAdditionalHeight = 45;
	MaxStepHeight = 45;
	SetWalkableFloorAngle(45);
	bUseFlatBaseForFloorChecks = false;
	DefaultLandMovementMode = MOVE_Walking;
	
	bUseSeparateBrakingFriction = false;
	MaxAcceleration = 8192;
	BrakingDecelerationWalking = 8192;
	
	AirControl = 0.1;
	
	GravityScale = 2;
	
	RotationRate = {0,500, 0};
}

void UBSPlayerMovementComponent::InitializeAsc(UAbilitySystemComponent* AbilitySystemComponent)
{
	OwnerAsc = AbilitySystemComponent;
	if (!OwnerAsc.IsValid()) return;
		
	UAbilitySystemComponent* Asc = OwnerAsc.Get();
	
	BindAttributes(Asc);
			
	BindTags(Asc);
}

void UBSPlayerMovementComponent::BindTags(UAbilitySystemComponent* InAsc)
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
	
	if (JumpUnlockTag.IsValid())
	{
		JumpTags = InAsc->GetGameplayTagCount(JumpUnlockTag);
		CharacterOwner->JumpMaxCount = JumpTags;
		
		InAsc->RegisterGameplayTagEvent(BSTags::Talent_Jump, EGameplayTagEventType::AnyCountChange).AddWeakLambda(
		this, [this](const FGameplayTag Tag, const int32 Count)
		{
			JumpTags = Count;
			CharacterOwner->JumpMaxCount = JumpTags;
		});	
	}
}

void UBSPlayerMovementComponent::BindAttributes(UAbilitySystemComponent* InAsc)
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
	
	if (FrictionAttribute.IsValid())
	{
		CachedAccelerationMod = InAsc->GetNumericAttribute(FrictionAttribute);
		
		InAsc->GetGameplayAttributeValueChangeDelegate(FrictionAttribute).AddWeakLambda(
		this, [this](const FOnAttributeChangeData& Data)
		{
			CachedAccelerationMod = Data.NewValue;
		});	
	}
	
	if (AirControlAttribute.IsValid())
	{
		AirControl = InAsc->GetNumericAttribute(AirControlAttribute);
		
		InAsc->GetGameplayAttributeValueChangeDelegate(AirControlAttribute).AddWeakLambda(
		this, [this](const FOnAttributeChangeData& Data)
		{
			AirControl = Data.NewValue;
		});
	}
	
	if (JumpHeightAttribute.IsValid())
	{	
		CachedJumpHeight = InAsc->GetNumericAttribute(JumpHeightAttribute);
		JumpZVelocity =  FMath::Sqrt(2.0f * -GetGravityZ() * CachedJumpHeight);
		
		InAsc->GetGameplayAttributeValueChangeDelegate(JumpHeightAttribute).AddWeakLambda(
			this, [this](const FOnAttributeChangeData& Data){
			CachedJumpHeight = Data.NewValue;
			JumpZVelocity =  FMath::Sqrt(2.0f * -GetGravityZ() * CachedJumpHeight);
		});
	}
}

bool UBSPlayerMovementComponent::CanAttemptJump() const
{
	return CanMove() && (Super::CanAttemptJump() || IsCoyoteTimeActive());
}

bool UBSPlayerMovementComponent::DoJump(const bool bReplayingMoves, const float DeltaTime)
{
	//From the Super::. Needed to modify directly for convenience with coyote time.
	if (CharacterOwner && (CharacterOwner->CanJump() || IsCoyoteTimeActive()))
	{
		if (!bConstrainToPlane || 
			!FMath::IsNearlyEqual(FMath::Abs(GetGravitySpaceZ(PlaneConstraintNormal)), 1.f))
		{
			if (const bool bFirstJump = CharacterOwner->JumpCurrentCountPreJump == 0; 
				bFirstJump || bDontFallBelowJumpZVelocityDuringJump)
			{
				if (HasCustomGravity())
				{
					SetGravitySpaceZ(Velocity, FMath::Max<FVector::FReal>(GetGravitySpaceZ(Velocity), JumpZVelocity));
				}
				else
				{
					Velocity.Z = FMath::Max<FVector::FReal>(Velocity.Z, JumpZVelocity);
				}
			}
			
			if (IsCoyoteTimeActive())
			{
				ConsumeCoyoteTime();
			}
			
			if (!IsMovingOnGround()) //Double jump snap to input direction. GetCurrentAcceleration is replicated
			{
				const float VerticalVelocity = Velocity.Z;
				Velocity = GetCurrentAcceleration().GetSafeNormal() * GetMaxSpeed();
				Velocity.Z = VerticalVelocity;
			}
			
			SetMovementMode(MOVE_Falling);
			return true;
		}
	}
	
	return false;
}

bool UBSPlayerMovementComponent::CanMove() const
{
	return ImpairmentTags.IsEmpty();
}

float UBSPlayerMovementComponent::GetMaxAcceleration() const
{
	return Super::GetMaxAcceleration() * CachedAccelerationMod;
}

float UBSPlayerMovementComponent::GetMaxBrakingDeceleration() const
{
	return Super::GetMaxBrakingDeceleration() * CachedAccelerationMod;
}

float UBSPlayerMovementComponent::GetMaxSpeed() const
{
	if (!CanMove()) return 0;
	//1 is default if no Asc has been found. Otherwise, it will be the GetSpeedAttribute().
	return Super::GetMaxSpeed() * CachedSpeedMod;
}

void UBSPlayerMovementComponent::HandleWalkingOffLedge(const FVector& PreviousFloorImpactNormal,
	const FVector& PreviousFloorContactNormal, const FVector& PreviousLocation, float TimeDelta)
{
	Super::HandleWalkingOffLedge(PreviousFloorImpactNormal, PreviousFloorContactNormal, PreviousLocation, TimeDelta);
	StartCoyoteTime();
}

void UBSPlayerMovementComponent::StartCoyoteTime()
{
	CoyoteStartTime = GetWorld()->GetTimeSeconds();
}

bool UBSPlayerMovementComponent::IsCoyoteTimeActive() const
{
	return !IsMovingOnGround() && JumpTags > 0 && 
		GetWorld()->GetTimeSeconds() - CoyoteTimeDuration < CoyoteStartTime;
}

void UBSPlayerMovementComponent::ConsumeCoyoteTime()
{
	CharacterOwner->JumpCurrentCount = 0;
	CoyoteStartTime = 0;
}

