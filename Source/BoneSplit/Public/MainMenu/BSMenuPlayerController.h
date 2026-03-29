// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BSMenuPlayerController.generated.h"

class UCommonActivatableWidgetSwitcher;
class UBSWidget_MainMenuRoot;
/**
 * 
 */
UCLASS(Abstract, Blueprintable, BlueprintType)
class BONESPLIT_API ABSMenuPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	
	virtual void BeginPlay() override;
	
protected:
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TSubclassOf<UBSWidget_MainMenuRoot> MainMenuRootClass;
	
	UPROPERTY(Transient)
	TObjectPtr<UBSWidget_MainMenuRoot> MainMenuRootInstance = nullptr;
};
