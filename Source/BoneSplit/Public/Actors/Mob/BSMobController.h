// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BSMobController.generated.h"

UCLASS()
class BONESPLIT_API ABSMobController : public AAIController
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABSMobController();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
