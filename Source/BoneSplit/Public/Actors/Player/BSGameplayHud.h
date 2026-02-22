// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "BSGameplayHud.generated.h"

class UBSCharacterPane;
class UBSPauseMenu;
class UBSWDamageNumber;
class UBSWHud;
class UBSGameplayRootWidget;
struct FGameplayEventData;
class UBSWRoot;
/**
 * 
 */
UCLASS(Abstract, BlueprintType, Blueprintable)
class BONESPLIT_API ABSGameplayHud : public AHUD
{
	GENERATED_BODY()
	
public:
	
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	void SpawnWidgets();
	
	virtual void SpawnDamageNumber(FGameplayEventData EventData);
	
	virtual bool IsInPauseMenu();
	
	//Adds pause menu to screen. This widget should handle back by itself.
	virtual void TogglePauseWidget();
	
	//Toggles the character pane.
	virtual void ToggleCharacterPaneWidget();
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TSubclassOf<UBSWHud> HudWidgetClass;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TSubclassOf<UBSCharacterPane> CharacterPaneClass;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TSubclassOf<UBSPauseMenu> PauseMenuClass;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TSubclassOf<UBSWDamageNumber> FloatingDamageNumberClass;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TSubclassOf<UBSGameplayRootWidget> GameplayRootWidgetClass;
	
	UPROPERTY()
	UBSGameplayRootWidget* GameplayRootWidgetInstance;
};
