// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/WorldSettings.h"
#include "BSWorldSettings.generated.h"

/**
 * 
 */
UCLASS()
class BONESPLIT_API ABSWorldSettings : public AWorldSettings
{
	GENERATED_BODY()
	
public:
	
	explicit ABSWorldSettings(const FObjectInitializer& Initializer);
};
