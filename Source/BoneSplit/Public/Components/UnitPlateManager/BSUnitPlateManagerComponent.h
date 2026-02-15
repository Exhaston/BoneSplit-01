// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BSUnitPlateManagerComponent.generated.h"


class UAbilitySystemComponent;
class UBSFloatingNamePlate;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class BONESPLIT_API UBSUnitPlateManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	
	UBSUnitPlateManagerComponent();
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TSubclassOf<UBSFloatingNamePlate> NamePlateWidgetClass;
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
	
	UPROPERTY()
	TMap<TObjectPtr<UAbilitySystemComponent>, UBSFloatingNamePlate*> UnitPlateInstances;
};
