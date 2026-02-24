// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/StreamableManager.h"
#include "BSEquipmentMeshComponent.generated.h"

/**
 * 
 */

class UBSEquipmentMeshComponent;

DECLARE_MULTICAST_DELEGATE_TwoParams(FBSOnSkeletalMeshSet, FGameplayTag MeshTag, USkeletalMesh* NewMesh);

UCLASS()
class BONESPLIT_API UBSEquipmentMeshComponent : public USkeletalMeshComponent
{
	GENERATED_BODY()
	
public:
	
	UBSEquipmentMeshComponent();
	
	virtual void InitializeEquipmentMesh(FGameplayTag InMeshTag, TSoftObjectPtr<USkeletalMesh> InMeshAsset, FColor InColor);
	
	virtual void SetColor(FColor Color);
	
	virtual void LazyLoadSkeletalMesh(TSoftObjectPtr<USkeletalMesh> MeshAsset);
	
	FGameplayTag GetMeshTag() const { return MeshTag; }
	
	FBSOnSkeletalMeshSet& GetOnSkeletalMeshSet() { return OnSkeletalMeshSetDelegate; }
	
protected:
	
	UPROPERTY()
	FGameplayTag MeshTag;
	
	FBSOnSkeletalMeshSet OnSkeletalMeshSetDelegate;
	
	UPROPERTY()
	FColor CurrentColor = FColor::White;
	
	virtual void SetSkeletalMesh(USkeletalMesh* NewMesh, bool bReinitPose = true) override;
	
	TSharedPtr<FStreamableHandle> StreamingHandle;
};
