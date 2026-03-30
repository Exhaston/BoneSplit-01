// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Environment/BSKillZVolume.h"

#include "AbilitySystemComponent.h"
#include "DefaultAttributeSet.h"
#include "GameplayEffectTypes.h"
#include "Abilities/BSDamageEffect.h"
#include "Abilities/BSExtendedAttributeSet.h"
#include "Player/BSPlayerCharacterBase.h"

ABSKillZVolume::ABSKillZVolume(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void ABSKillZVolume::ActorEnteredVolume(AActor* Other)
{
	if (ABSPlayerCharacterBase* PlayerCharacterBase = Cast<ABSPlayerCharacterBase>(Other))
	{
		if (UAbilitySystemComponent* PlayerAsc = PlayerCharacterBase->GetAbilitySystemComponent(); PlayerAsc && PlayerCharacterBase->HasAuthority())
		{
			const FGameplayEffectSpecHandle SpecHandle = 
				PlayerAsc->MakeOutgoingSpec(UBSDamageEffect::StaticClass(), 1, PlayerAsc->MakeEffectContext());
		
			SpecHandle.Data.Get()->SetSetByCallerMagnitude(DefaultSetByCallerTags::SetByCaller_Damage, 25);
	
			SpecHandle.Data.Get()->SetSetByCallerMagnitude(BSExtendedAttributeTags::SetByCaller_Knockback, 0);
	
			PlayerAsc->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);
		}
			
		PlayerCharacterBase->TeleportToSafety();
	}
	else
	{
		Super::ActorEnteredVolume(Other);
	}
}
