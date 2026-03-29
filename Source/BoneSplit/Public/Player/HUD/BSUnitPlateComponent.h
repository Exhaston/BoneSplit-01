// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "BSUnitPlateComponent.generated.h"


class UAbilitySystemComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class BONESPLIT_API UBSUnitPlateComponent : public UWidgetComponent
{
	GENERATED_BODY()

public:
	UBSUnitPlateComponent();
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	float VisibilityDistance = 4000;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	float VisibilityUpdateInterval = 2;
	
	float CurrentUpdate = 0;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	bool bRequireVisibility = true;
	
	virtual void SetupUnitPlate(UAbilitySystemComponent* InAbilitySystemComponent, FText UnitName);
	
	virtual void SetActive(bool bNewActive, bool bReset = false) override;
	
	virtual void BeginPlay() override;
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
	
	bool bShouldBeVisible = false;
};
