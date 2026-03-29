// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "Widgets/CommonActivatableWidgetContainer.h"
#include "BSHudComponent.generated.h"


class UBSWidget_PauseMenu;
class UBSWidget_PlayerFrame;
class UBSWidget_HudRoot;
class UAbilitySystemComponent;
class UBSWidget_FloatingText;
class UCommonActivatableWidgetStack;
class UBSGameplayRootWidget;
class UBSPauseMenu;
class UBSCharacterPane;

UCLASS()
class BONESPLIT_API UBSHudComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	
	UBSHudComponent();
	
	template <typename T>
	static T* FindWidgetByClass(UCommonActivatableWidgetStack* InStack)
	{
		static_assert(TIsDerivedFrom<T, UCommonActivatableWidget>::Value,
			"TWidgetClass must derive from UCommonActivatableWidget");

		if (!IsValid(InStack))
		{
			return nullptr;
		}
		
		TArray<UCommonActivatableWidget*> AllWidgets 
		= InStack->GetWidgetList();
		
		for (UCommonActivatableWidget* Widget : AllWidgets)
		{
			if (T* Typed = Cast<T>(Widget))
			{
				return Typed;
			}
		}

		return nullptr;
	}
	
	static UBSHudComponent* GetHudComponent(const APlayerController* PC);
	
	virtual void SetupHud(APlayerController* OwnerController);
	
	virtual UCommonActivatableWidgetStack* GetFullscreenStack() const;
	
	virtual void SetPauseMenuActive();
	virtual void SetCharacterPaneActive(bool bActive = true);
	
	virtual void OnDamageDealt(
		UAbilitySystemComponent* Inst, 
		UAbilitySystemComponent* Target, 
		FGameplayTagContainer RelevantTags, 
		float BaseDamage, 
		float OverallDamage);
	
	virtual void OnHealingDealt(
		UAbilitySystemComponent* Inst, 
		UAbilitySystemComponent* Target, 
		FGameplayTagContainer RelevantTags, 
		float BaseHealing, 
		float OverallHealing);
	
	virtual void SpawnFloatingText(FVector WorldLocation, FText InText, FGameplayTagContainer RelevantTags);
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="UI")
	TSubclassOf<UBSCharacterPane> CharacterPaneClass;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="UI")
	TSubclassOf<UBSWidget_PauseMenu> PauseMenuClass;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="UI")
	TSubclassOf<UBSWidget_FloatingText> FloatingTextClass;
	
	//The root widget that holds the fullscreen stack and acts a single reference to the UI.
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="UI")
	TSubclassOf<UBSWidget_HudRoot> HudRootWidgetClass;
	
	virtual void SpawnPlayerFrame(UAbilitySystemComponent* InAsc, FString PlayerName);
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="UI")
	TSubclassOf<UBSWidget_PlayerFrame> PlayerFrameWidgetClass;
	
	virtual bool GetIsPaused() { return bIsPaused;}
	
protected:
	
	bool bIsPaused = false;
	
	UPROPERTY(Transient)
	TObjectPtr<APlayerController> OwnerPlayerController = nullptr;
	
	UPROPERTY(Transient)
	UBSWidget_HudRoot* GameplayRootWidgetInstance = nullptr;
};
