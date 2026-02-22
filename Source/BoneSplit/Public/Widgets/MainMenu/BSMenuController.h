// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BSMenuController.generated.h"

class UBSWMainMenuRoot;
/**
 * 
 */
UCLASS()
class BONESPLIT_API ABSMenuController : public APlayerController
{
	GENERATED_BODY()
	
public:
	
	virtual void BeginPlay() override;
	
	virtual void PreClientTravel(const FString& PendingURL, ETravelType TravelType, bool bIsSeamlessTravel) override;
	
protected:
	

};
