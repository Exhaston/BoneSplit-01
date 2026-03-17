// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BSReadyZone.generated.h"

class UBSGroupSubsystem;

UCLASS(Abstract, BlueprintType, Blueprintable)
class BONESPLIT_API ABSReadyZone : public AActor
{
	GENERATED_BODY()

public:
	
	ABSReadyZone();
	
	FSimpleMulticastDelegate OnAllPlayersReadyDelegate;
	
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	virtual void NotifyActorEndOverlap(AActor* OtherActor) override;
	
	UPROPERTY()
	bool bWasTriggered = false;
};
