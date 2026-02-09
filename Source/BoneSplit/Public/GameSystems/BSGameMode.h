// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "BSGameMode.generated.h"

class UBSGameManagerSubsystem;
/**
* Base Game Mode class that allows for transitions with specific spawn points
*/
UCLASS()
class BONESPLIT_API ABSGameMode : public AGameMode
{
	GENERATED_BODY()
	
public:
	
	explicit ABSGameMode(const FObjectInitializer& ObjectInitializer);
	
	UFUNCTION(BlueprintAuthorityOnly, BlueprintCallable, DisplayName="LoadMap")
	void BP_LoadMap(TSoftObjectPtr<UWorld> InMap, FString NextPlayerStartTag) const;
	
	virtual AActor* FindPlayerStart_Implementation(AController* Player, const FString& IncomingName = L"") override;
};