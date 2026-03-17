// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BSSpawner.generated.h"

class ABSMobCharacter;

//Abstract because visual elements in editor is important, subclass this in a bp.
UCLASS(Abstract, Blueprintable, BlueprintType)
class BONESPLIT_API ABSSpawner : public AActor
{
	GENERATED_BODY()

public:
	
	ABSSpawner();

	// Spawns a single mob of the given class at this spawner's location/rotation
	// Returns the spawned character, or nullptr on failure
	UFUNCTION(BlueprintCallable)
	ABSMobCharacter* SpawnMob(TSubclassOf<ABSMobCharacter> MobClass);

protected:
	
	virtual void BeginPlay() override;

	// Optional: scene component to mark the actual spawn point offset
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USceneComponent> SpawnPoint;
};
