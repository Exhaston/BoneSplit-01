// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "BSFloatingUnitPlateSubsystem.generated.h"

class UWidgetComponent;
class UBSFloatingNamePlate;
/**
 * 
 */
UCLASS()
class BONESPLIT_API UBSFloatingUnitPlateSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()
	
public:
	
	virtual bool IsTickable() const override;
	
	virtual TStatId GetStatId() const override;
	
	virtual void Tick(float DeltaTime) override;
	
	virtual void RegisterFloatingNamePlate(UWidgetComponent* FloatingNamePlate);
	virtual void UnRegisterFloatingNamePlate(UWidgetComponent* FloatingNamePlate);
	
	UPROPERTY()
	TArray<TWeakObjectPtr<UWidgetComponent>> RegisteredFloatingNamePlates;
};
