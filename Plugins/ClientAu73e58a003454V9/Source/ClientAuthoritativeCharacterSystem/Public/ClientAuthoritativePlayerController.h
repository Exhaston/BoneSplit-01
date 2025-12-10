// Copyright 2023 Anton Hesselbom. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ClientAuthoritativePlayerController.generated.h"

// Intended to be used together with AClientAuthoritativeCharacter & UClientAuthoritativeCharacterMovementComponent
UCLASS()
class CLIENTAUTHORITATIVECHARACTERSYSTEM_API AClientAuthoritativePlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void TickActor(float DeltaTime, ELevelTick TickType, FActorTickFunction& ThisTickFunction) override;
};
