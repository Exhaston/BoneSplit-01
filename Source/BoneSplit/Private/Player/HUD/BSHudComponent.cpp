// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Player/HUD/BSHudComponent.h"

#include "CharacterAbilitySystem.h"
#include "Widgets/CommonActivatableWidgetContainer.h"
#include "Player/HUD/BSWidget_PauseMenu.h"
#include "Widgets/HUD/BSWHud.h"
#include "Player/HUD/BSWidget_FloatingText.h"
#include "Player/HUD/BSWidget_HudRoot.h"
#include "Player/HUD/BSWidget_PlayerFrame.h"
#include "UserSettings/BSWidget_SettingsRoot.h"

UBSHudComponent::UBSHudComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

UBSHudComponent* UBSHudComponent::GetHudComponent(const APlayerController* PC)
{
	return PC->GetComponentByClass<UBSHudComponent>();
}

void UBSHudComponent::SetupHud(APlayerController* OwnerController)
{
	if (!OwnerController->IsLocalPlayerController()) return;
	
	OwnerPlayerController = OwnerController;
	if (GameplayRootWidgetInstance)
	{
		GameplayRootWidgetInstance->RemoveFromParent();
		GameplayRootWidgetInstance = nullptr;
	}
		
	GameplayRootWidgetInstance = CreateWidget<UBSWidget_HudRoot>(OwnerController, HudRootWidgetClass);
	GameplayRootWidgetInstance->AddToPlayerScreen();
	
	if (const IAbilitySystemInterface* OwnerAscI = Cast<IAbilitySystemInterface>(OwnerController))
	{
		UCharacterAbilitySystem* OwnerAbilitySystem = 
			Cast<UCharacterAbilitySystem>(OwnerAscI->GetAbilitySystemComponent());
		
		if (!OwnerAbilitySystem) return;
		
		OwnerAbilitySystem->OnDamageDealt.RemoveAll(this);
		OwnerAbilitySystem->OnDamageDealt.AddUObject(this, &UBSHudComponent::OnDamageDealt);
		
		OwnerAbilitySystem->OnHealingDealt.RemoveAll(this);
		OwnerAbilitySystem->OnHealingDealt.AddUObject(this, &UBSHudComponent::OnHealingDealt);
	}
}

UCommonActivatableWidgetStack* UBSHudComponent::GetFullscreenStack() const
{
	return GameplayRootWidgetInstance ? GameplayRootWidgetInstance->WidgetStack : nullptr;
}

void UBSHudComponent::SetPauseMenuActive()
{
	if (!GetFullscreenStack()->GetWidgetList().ContainsByPredicate([this](const UCommonActivatableWidget* Widget)
	{
		return Widget->IsA(UBSWidget_PauseMenu::StaticClass());
	}))
	{
		for (auto Element : GetFullscreenStack()->GetWidgetList())
		{
			Element->DeactivateWidget();
		}
		GetFullscreenStack()->AddWidget<UBSWidget_PauseMenu>(PauseMenuClass);
	}
}

void UBSHudComponent::SetCharacterPaneActive(bool bActive)
{
}

void UBSHudComponent::OnDamageDealt(UAbilitySystemComponent* Inst, UAbilitySystemComponent* Target,
	const FGameplayTagContainer RelevantTags, float BaseDamage, const float OverallDamage)
{
	const AActor* TargetActor = Target->GetAvatarActor();
	if (!TargetActor) return;
	
	FVector Offset = FVector::ZeroVector;
	Offset.Z = TargetActor->GetSimpleCollisionHalfHeight() + 100;
	
	FNumberFormattingOptions Options;
	Options.MinimumFractionalDigits = 0;
	Options.MaximumFractionalDigits = 0;

	SpawnFloatingText(TargetActor->GetActorLocation() + Offset, FText::AsNumber(OverallDamage, &Options), RelevantTags);
}

void UBSHudComponent::OnHealingDealt(UAbilitySystemComponent* Inst, UAbilitySystemComponent* Target,
FGameplayTagContainer RelevantTags,	float BaseHealing, const float OverallHealing)
{
	const AActor* TargetActor = Target->GetAvatarActor();
	if (!TargetActor) return;
	
	RelevantTags.AddTagFast(BSGameplayTags::Widget_FloatingText_Helpful);
	
	FVector Offset = FVector::ZeroVector;
	Offset.Z = TargetActor->GetSimpleCollisionHalfHeight() + 100;
			
	SpawnFloatingText(TargetActor->GetActorLocation() + Offset, FText::AsNumber(OverallHealing), RelevantTags);
}

void UBSHudComponent::SpawnFloatingText(
	const FVector WorldLocation, const FText InText, const FGameplayTagContainer RelevantTags)
{
	if (!GameplayRootWidgetInstance) return; 
	UBSWidget_FloatingText* FloatingTextInstance = 
		CreateWidget<UBSWidget_FloatingText>(GameplayRootWidgetInstance, FloatingTextClass);
	
	FloatingTextInstance->AddToPlayerScreen();
	
	FloatingTextInstance->SetFloatingText(WorldLocation, InText, RelevantTags);
}

void UBSHudComponent::SpawnPlayerFrame(UAbilitySystemComponent* InAsc, const FString PlayerName)
{
	if (!PlayerFrameWidgetClass) return; 
	UBSWidget_PlayerFrame* FloatingTextInstance = 
		CreateWidget<UBSWidget_PlayerFrame>(GameplayRootWidgetInstance, FloatingTextClass);
	
	FloatingTextInstance->AddToPlayerScreen();
	
	FloatingTextInstance->SetPlayerAbilitySystem(InAsc, PlayerName);
}

