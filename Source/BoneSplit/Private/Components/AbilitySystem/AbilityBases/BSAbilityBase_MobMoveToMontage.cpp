// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Components/AbilitySystem/AbilityBases/BSAbilityBase_MobMoveToMontage.h"

void UBSAbilityBase_MobMoveToMontage::StartMoveToAbility()
{
	/*
	if (TagEffectMap.Contains(GameplayTag) && Payload.Target && 
	HasAuthorityOrPredictionKey(GetCurrentActorInfo(), &GetCurrentActivationInfoRef()))
	{
		const TSubclassOf<UGameplayEffect> AppliedEffectClass = TagEffectMap[GameplayTag];
		
		if (const IAbilitySystemInterface* AscInterface = Cast<IAbilitySystemInterface>(Payload.Target))
		{
			if (!UBSAbilityFunctionLibrary::FilterByNoMatchingFactions(
				GetAbilitySystemComponentFromActorInfo(), 
				AscInterface->GetAbilitySystemComponent())) return;
			
			if (!UBSAbilityFunctionLibrary::CheckTargetVisibility(
				GetAvatarActorFromActorInfo(), 
				GetAvatarActorFromActorInfo()->GetActorLocation(), 
				Payload.Target)) return;
			
			UBSAbilityFunctionLibrary::LazyApplyEffectToTarget(
				this, 
				AscInterface->GetAbilitySystemComponent(), 
				AppliedEffectClass, 
				GetAvatarActorFromActorInfo()->GetActorLocation(),
				GetAbilityLevel());
		}
	}
	*/
}
