// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Base/BSWRoot.h"
#include "CommonActivatableWidget.h"
#include "NativeGameplayTags.h"
#include "HUD/BSWDamageNumber.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "Widgets/CommonActivatableWidgetContainer.h"
#include "BSLocalWidgetSubsystem.generated.h"

class UBSFloatingNamePlate;
class UBSWServerBrowser;
class UBSWUserConfirmContext;
class UBSCharacterPane;
class UBSPauseMenu;
class UBSWHud;
class UBSWRoot;

DECLARE_MULTICAST_DELEGATE(FBSWidgetDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FBSWidgetDelegateDynamic);

/**
 * Widget Subsystem that handles loading classes from BSDeveloperSettings. 
 * As long as the classes are set they should be valid as long as the game instance is.
 */
UCLASS()
class BONESPLIT_API UBSLocalWidgetSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	
	UBSLocalWidgetSubsystem();
	
	static UBSLocalWidgetSubsystem* GetWidgetSubsystem(const UObject* Context);
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	
	static void GetOrSetAbilitySystem(const UUserWidget* WidgetInstance, TWeakObjectPtr<UAbilitySystemComponent>& PointerToSet);
	
	static bool IsWidgetActive(UCommonActivatableWidgetStack* StackToCheck, TSubclassOf<UCommonActivatableWidget> WidgetClass);
	
	//Removes an activatable widget from the stack by class
	static void RemoveWidgetFromStack(UCommonActivatableWidgetStack* StackToCheck, TSubclassOf<UCommonActivatableWidget> WidgetInstance);
	
	UPROPERTY()
	bool bIsPaused = false;
	
	//Function to get the current blueprint authored widget from project settings.
	template<typename T>
	TSubclassOf<T> GetBlueprintOverrideClass(const FGameplayTag Tag) const
	{
		if (UClass* const* Value = WidgetClassOverrides.Find(Tag))
			return Value;

		return nullptr;
	}
	
	UPROPERTY()
	TMap<FGameplayTag, UClass*> WidgetClassOverrides;
};
