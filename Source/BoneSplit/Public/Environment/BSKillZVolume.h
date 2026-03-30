// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/KillZVolume.h"
#include "BSKillZVolume.generated.h"

UCLASS(BlueprintType, Blueprintable, DisplayName="Kill Volume")
class BONESPLIT_API ABSKillZVolume : public AKillZVolume
{
	GENERATED_BODY()

public:
	
	ABSKillZVolume(const FObjectInitializer& ObjectInitializer);
	
	virtual void ActorEnteredVolume(AActor* Other) override;
};
