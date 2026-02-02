// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BSWRoot.h"
#include "CommonActivatableWidget.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "Widgets/CommonActivatableWidgetContainer.h"
#include "BSLocalWidgetSubsystem.generated.h"

class UBSWHud;
class UBSWRoot;
/**
 * 
 */
UCLASS()
class BONESPLIT_API UBSLocalWidgetSubsystem : public ULocalPlayerSubsystem
{
	GENERATED_BODY()
	
public:
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	
	//Initializes the player UI Root Widget. This is required to push and remove widgets.
	virtual UBSWRoot* CreatePlayerUI(APlayerController* InPlayerController);
	
	//Adds an activatable widget to the main stack.
	template <class T>
	T* AddWidgetToStack(TSubclassOf<T> WidgetToAdd) {
		check(RootWidgetInstance);
		static_assert(std::is_base_of_v<UCommonActivatableWidget, T>,
			"T must be a subclass of UCommonActivatableWidget");
		return RootWidgetInstance->WidgetStack->AddWidget<T>(WidgetToAdd);
	}
	
	//Removes an activatable widget from the stack by class
	void RemoveWidgetFromStack(TSubclassOf<UCommonActivatableWidget> WidgetInstance) const;
	
	UPROPERTY(Transient)
	UBSWRoot* RootWidgetInstance;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TSubclassOf<UBSWRoot> RootWidgetClass;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TSubclassOf<UBSWHud> HudWidgetClass;
};
