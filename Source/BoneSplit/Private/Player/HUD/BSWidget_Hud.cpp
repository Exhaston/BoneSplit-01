// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Player/HUD/BSWidget_Hud.h"

#include "AbilitySystemInterface.h"
#include "Abilities/BSExtendedAttributeSet.h"
#include "GameFramework/PlayerState.h"
#include "Player/HUD/BSWidget_AttributeBar.h"
#include "Player/HUD/BSWidget_PlayerFrame.h"

void UBSWidget_Hud::NativeConstruct()
{
	Super::NativeConstruct();

	const IAbilitySystemInterface* AscI = GetOwningPlayer<IAbilitySystemInterface>();
	if (!AscI) return;
	UAbilitySystemComponent* OwnerAsc = AscI->GetAbilitySystemComponent();
	if (!OwnerAsc) return;
	
	PlayerFrameWidget->SetPlayerAbilitySystem(OwnerAsc, GetOwningPlayerState()->GetPlayerName());
	
	SoulBar->SetAbilitySystemToTrack(UBSExtendedAttributeSet::GetSoulChargeAttribute(), UBSExtendedAttributeSet::GetMaxSoulChargeAttribute(), OwnerAsc);
}
