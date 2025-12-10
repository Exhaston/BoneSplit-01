// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Components/Inventory/BSEquipmentMeshComponent.h"

#include "BoneSplit/BoneSplit.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "GameSettings/BSDeveloperSettings.h"

static FName ColorParamName = FName("Color");

void UBSEquipmentMeshComponent::SetMeshColorIndex(const int32 Index)
{
	CurrentColor = Index;
	const UBSDeveloperSettings* DeveloperSettings = GetDefault<UBSDeveloperSettings>();
	SetVectorParameterValueOnMaterials(
	ColorParamName, FVector(DeveloperSettings->PlayerColors[CurrentColor]));
}

void UBSEquipmentMeshComponent::LazyLoadSkeletalMesh(TSoftObjectPtr<USkeletalMesh> MeshAsset)
{
	if (MeshAsset.IsValid() && MeshAsset.Get() == GetSkeletalMeshAsset()) return; //The same asset. No need to reload
	
	FStreamableManager& Streamable = UAssetManager::GetStreamableManager();

	Streamable.RequestAsyncLoad(
		MeshAsset.ToSoftObjectPath(),
		FStreamableDelegate::CreateWeakLambda(this, [this, MeshAsset]()
		{
			if (IsBeingDestroyed() || !MeshAsset.IsValid())
			{
				UE_LOG(BoneSplit, Warning, TEXT("Failed to async-load skeletal mesh: %s"), *MeshAsset.ToString());
				return;
			}
			SetSkeletalMeshAsset(MeshAsset.Get());
			const UBSDeveloperSettings* DeveloperSettings = GetDefault<UBSDeveloperSettings>();
			SetVectorParameterValueOnMaterials(
			ColorParamName, FVector(DeveloperSettings->PlayerColors[CurrentColor]));
		})
	);
}
