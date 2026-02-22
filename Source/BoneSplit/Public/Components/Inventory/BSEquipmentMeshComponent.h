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

DECLARE_MULTICAST_DELEGATE_OneParam(FBSOnSkeletalMeshSet, USkeletalMesh* NewMest);

UCLASS()
class BONESPLIT_API UBSEquipmentMeshComponent : public USkeletalMeshComponent
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta=(AllowPrivateAccess=true, Categories="EquipmentMesh"))
	FGameplayTag MeshTag;
	
	virtual void SetColor(FColor Color);
	
	FBSOnSkeletalMeshSet OnSkeletalMeshSetDelegate;
	
	UPROPERTY()
	FColor CurrentColor = FColor::White;
	
	virtual void LazyLoadSkeletalMesh(TSoftObjectPtr<USkeletalMesh> MeshAsset);
	
	virtual void SetSkeletalMesh(USkeletalMesh* NewMesh, bool bReinitPose = true) override;
	
	TSharedPtr<FStreamableHandle> StreamingHandle;
};
