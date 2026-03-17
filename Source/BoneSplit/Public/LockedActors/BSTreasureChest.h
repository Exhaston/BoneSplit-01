// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BSTreasureChest.generated.h"

UCLASS()
class BONESPLIT_API ABSTreasureChest : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABSTreasureChest();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
