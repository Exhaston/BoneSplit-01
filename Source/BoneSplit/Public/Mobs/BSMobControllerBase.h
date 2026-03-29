// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MMobController.h"
#include "BSMobControllerBase.generated.h"

UCLASS()
class BONESPLIT_API ABSMobControllerBase : public AMMobController
{
	GENERATED_BODY()
	
public:
	
	virtual bool IsActorAlly(AActor* InActor) override;
	virtual bool IsActorEnemy(AActor* InActor) override;
};
