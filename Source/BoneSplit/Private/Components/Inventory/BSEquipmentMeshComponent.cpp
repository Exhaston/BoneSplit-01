// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Components/Inventory/BSEquipmentMeshComponent.h"

#include "BoneSplit/BoneSplit.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "GameSettings/BSDeveloperSettings.h"

static FName ColorParamName = FName("Color");

void UBSEquipmentMeshComponent::SetColor(const FColor InColor)
{
	CurrentColor = InColor;
	SetVectorParameterValueOnMaterials(ColorParamName, FVector(CurrentColor));
}

void UBSEquipmentMeshComponent::LazyLoadSkeletalMesh(TSoftObjectPtr<USkeletalMesh> MeshAsset)
{
	if (StreamingHandle.IsValid() && StreamingHandle.Get()->IsLoadingInProgress()) 
	{
		StreamingHandle.Get()->CancelHandle(); //Already loading another mesh, cancel to load new
	}

	if (MeshAsset.IsNull())
	{
		SetSkeletalMesh(nullptr); //Clears the mesh
		return;
	}

	if (MeshAsset.IsValid()) //Already loaded, set directly
	{
		SetSkeletalMeshAsset(MeshAsset.Get());
		return;
	}

	FStreamableManager& Streamable = UAssetManager::GetStreamableManager();

	StreamingHandle = Streamable.RequestAsyncLoad(
		MeshAsset.ToSoftObjectPath(),
		FStreamableDelegate::CreateWeakLambda(this, [this, MeshAsset]()
		{
			if (IsBeingDestroyed() || !MeshAsset.IsValid())
			{
				UE_LOG(BoneSplit, Warning, TEXT("Failed to async-load skeletal mesh: %s"), *MeshAsset.ToString());
				return;
			}
			SetSkeletalMeshAsset(MeshAsset.Get());
		})
	);
}

void UBSEquipmentMeshComponent::SetSkeletalMesh(USkeletalMesh* NewMesh, bool bReinitPose)
{
	Super::SetSkeletalMesh(NewMesh, bReinitPose);
	
#if WITH_EDITOR
	CleanUpOverrideMaterials();
#endif
	EmptyOverrideMaterials();
	
	SetVectorParameterValueOnMaterials(ColorParamName, FVector(CurrentColor));
	OnSkeletalMeshSetDelegate.Broadcast(NewMesh);
}
