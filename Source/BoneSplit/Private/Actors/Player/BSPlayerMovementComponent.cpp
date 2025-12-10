// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Actors/Player/BSPlayerMovementComponent.h"

#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "BoneSplit/BoneSplit.h"
#include "Components/AbilitySystem/BSAttributeSet.h"

UBSPlayerMovementComponent::UBSPlayerMovementComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UBSPlayerMovementComponent::BeginPlay()
{
	Super::BeginPlay();
	
	if (const IAbilitySystemInterface* AscInterface = Cast<IAbilitySystemInterface>(GetOwner()))
	{
		OwnerAsc = AscInterface->GetAbilitySystemComponent();
		if (OwnerAsc.IsValid())
		{
			CachedSpeedMod = OwnerAsc.Get()->GetNumericAttribute(UBSAttributeSet::GetSpeedAttribute());
			
			OwnerAsc.Get()->GetGameplayAttributeValueChangeDelegate(UBSAttributeSet::GetSpeedAttribute()).AddWeakLambda(
			this, [this] (const FOnAttributeChangeData& Data)
			{
				CachedSpeedMod = Data.NewValue;
				
				if (GetOwner()->HasAuthority()) //Auto add slowed tag when we are below 1 in SpeedMod.
				{
					if (CachedSpeedMod < 1)
					{
						OwnerAsc.Get()->AddLooseGameplayTag(
							BSTags::Status_Slowed, 1, EGameplayTagReplicationState::TagAndCountToAll);
					}
					else
					{
						OwnerAsc.Get()->RemoveLooseGameplayTag(
							BSTags::Status_Slowed, 1, EGameplayTagReplicationState::TagAndCountToAll);
					}
				}
			});
			
			OwnerAsc.Get()->RegisterGameplayTagEvent(BSTags::Status_Stunned).AddWeakLambda(
			this, [this](const FGameplayTag Tag, const int32 Count)
			{
				bMovementImpaired = Count > 0;
			}); 		
			OwnerAsc.Get()->RegisterGameplayTagEvent(BSTags::Status_Dead).AddWeakLambda(
			this, [this](const FGameplayTag Tag, const int32 Count)
			{
				bDead = Count > 0;
			});
		}
	}
}

float UBSPlayerMovementComponent::GetMaxSpeed() const
{
	if (bDead || bMovementImpaired) return 0;
	//1 is default if no Asc has been found. Otherwise, it will be the GetSpeedAttribute().
	return Super::GetMaxSpeed() * CachedSpeedMod;
}

