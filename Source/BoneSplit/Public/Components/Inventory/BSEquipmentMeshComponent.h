// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/SkeletalMeshComponent.h"
#include "BSEquipmentMeshComponent.generated.h"

/**
 * 
 */
UCLASS()
class BONESPLIT_API UBSEquipmentMeshComponent : public USkeletalMeshComponent
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta=(AllowPrivateAccess=true))
	FGameplayTag MeshTag;
	
	virtual void SetMeshColorIndex(int32 Index);
	
	UPROPERTY()
	int32 CurrentColor = 0;
	
	virtual void LazyLoadSkeletalMesh(TSoftObjectPtr<USkeletalMesh> MeshAsset);
};
