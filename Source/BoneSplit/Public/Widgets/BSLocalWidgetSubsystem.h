// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Base/BSWRoot.h"
#include "CommonActivatableWidget.h"
#include "HUD/BSWDamageNumber.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "Widgets/CommonActivatableWidgetContainer.h"
#include "BSLocalWidgetSubsystem.generated.h"

class UBSWUserConfirmContext;
class UBSCharacterPane;
class UBSPauseMenu;
class UBSWHud;
class UBSWRoot;

DECLARE_MULTICAST_DELEGATE(FBSWidgetDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FBSWidgetDelegateDynamic);
/**
 * 
 */
UCLASS()
class BONESPLIT_API UBSLocalWidgetSubsystem : public ULocalPlayerSubsystem
{
	GENERATED_BODY()
	
public:
	
	static UBSLocalWidgetSubsystem* GetWidgetSubsystem(const UUserWidget* Context);
	static UBSLocalWidgetSubsystem* GetWidgetSubsystem(const APlayerController* Context);
	static UBSLocalWidgetSubsystem* GetWidgetSubsystem(const ULocalPlayer* Context);
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	
	//Only called when reloading worlds or cleaning up from.
	virtual void ClearWidgets();
	
	template <class T>
	T* CreateWidgetFromClass(TSubclassOf<T> WidgetToAdd)
	{
		return CreateWidget<T>(GetLocalPlayer()->GetPlayerController(GetWorld()), WidgetToAdd);
	}
	
	UBSWToolTipBase* CreateGenericToolTip(
		const FText& Header, const FText& Text, const FText& AltHeader = FText::GetEmpty());
	
	//Initializes the player UI Root Widget. This is required to push and remove widgets.
	virtual UBSWRoot* CreatePlayerUI(APlayerController* InPlayerController, bool bMainMenu = false);
	
	//Adds an activatable widget to the main stack.
	template <class T>
	T* AddWidgetToStack(TSubclassOf<T> WidgetToAdd) {
		check(RootWidgetInstance);
		static_assert(std::is_base_of_v<UCommonActivatableWidget, T>,
			"T must be a subclass of UCommonActivatableWidget");
		
		if (RootWidgetInstance->WidgetStack->GetWidgetList().ContainsByPredicate(
			[WidgetToAdd](const UCommonActivatableWidget* Widget)
		{
				return Widget->IsA(WidgetToAdd);
		})) return nullptr; //Already contains               
		
		return RootWidgetInstance->WidgetStack->AddWidget<T>(WidgetToAdd);
	}
	
	virtual void SpawnDamageNumber(FGameplayEventData EventData);
	
	virtual bool IsWidgetActive(TSubclassOf<UCommonActivatableWidget> WidgetClass);
	
	//Removes an activatable widget from the stack by class
	void RemoveWidgetFromStack(TSubclassOf<UCommonActivatableWidget> WidgetInstance) const;
	
	UBSWRoot* GetRootWidgetInstance();
	
	UPROPERTY(Transient)
	UBSWRoot* RootWidgetInstance;
	
	UPROPERTY()
	bool bIsPaused = false;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TSubclassOf<UBSWRoot> GameRootWidgetClass;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TSubclassOf<UBSWRoot> MainMenuWidgetRootClass;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TSubclassOf<UBSWHud> HudWidgetClass;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TSubclassOf<UBSCharacterPane> CharacterPaneWidgetClass;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TSubclassOf<UBSPauseMenu> PauseMenuWidgetClass;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TSubclassOf<UBSWToolTipBase> DefaultToolTipWidgetClass;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TSubclassOf<UBSWDamageNumber> DamageNumberWidgetClass;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TSubclassOf<UBSWUserConfirmContext> UserConfirmWidgetClass;
};
