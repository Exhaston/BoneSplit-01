// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/DecalComponent.h"
#include "BSDynamicDecalComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class BONESPLIT_API UBSDynamicDecalComponent : public UDecalComponent
{
	GENERATED_BODY()

public:
	UBSDynamicDecalComponent();
	
	//Sets up and registers the component. The dynamic material can be accesses through GetDynamicMaterial()
	virtual void RegisterDynamicDecal(UMaterialInterface* InMaterial);
	
	UMaterialInstanceDynamic* GetDynamicMaterial() const;

protected:
	
	UPROPERTY()
	UMaterialInstanceDynamic* DynamicMaterial;
};
