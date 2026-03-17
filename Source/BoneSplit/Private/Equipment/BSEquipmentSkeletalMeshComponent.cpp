// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Equipment/BSEquipmentSkeletalMeshComponent.h"

#include "BoneSplit/BoneSplit.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"

void UBSEquipmentSkeletalMeshComponent::SetSkeletalMesh(USkeletalMesh* NewMesh, const bool bReinitPose)
{
	Super::SetSkeletalMesh(NewMesh, bReinitPose);
	
	if (NewMesh)
	{
		SetVectorParameterValueOnMaterials(ColorParamName, FVector(MaterialColor));
	}
	
	if (OnSkeletalMeshLoaded.IsBound())
	{
		OnSkeletalMeshLoaded.Broadcast(NewMesh);
	}
}

void UBSEquipmentSkeletalMeshComponent::LoadSkeletalMeshAsync(const TSoftObjectPtr<USkeletalMesh> MeshAsset)
{
	if (CurrentMeshBeingLoaded == MeshAsset) return;
	
	CancelLoading();
	
	CurrentMeshBeingLoaded = MeshAsset;
	
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

	MeshAsyncLoadHandle = Streamable.RequestAsyncLoad(
		MeshAsset.ToSoftObjectPath(),
		FStreamableDelegate::CreateWeakLambda(this, [this]()
		{
			if (IsBeingDestroyed()) return;

			if (!CurrentMeshBeingLoaded.IsValid())
			{
				UE_LOG(BoneSplit, Warning, TEXT("Async load completed but mesh is no longer valid: %s"),
					*CurrentMeshBeingLoaded.ToString());
				return;
			}
			
			SetSkeletalMeshAsset(CurrentMeshBeingLoaded.Get());
		})
	);
}

void UBSEquipmentSkeletalMeshComponent::LoadSkeletalMeshSync(const TSoftObjectPtr<USkeletalMesh> MeshAsset)
{
	CancelLoading();
	
	if (MeshAsset.IsNull()) return;
	
	if (MeshAsset.IsValid())
	{
		SetSkeletalMeshAsset(MeshAsset.Get());
		return;
	}
	
	SetSkeletalMesh(MeshAsset.LoadSynchronous());
}

void UBSEquipmentSkeletalMeshComponent::SetMaterialColor(const FColor& NewColor)
{
	MaterialColor = NewColor;
	SetVectorParameterValueOnMaterials(ColorParamName, FVector(NewColor));
}

bool UBSEquipmentSkeletalMeshComponent::IsLoadingInProgress() const
{
	return MeshAsyncLoadHandle.IsValid() && MeshAsyncLoadHandle.Get()->IsLoadingInProgress();
}

void UBSEquipmentSkeletalMeshComponent::CancelLoading()
{
	if (IsLoadingInProgress()) 
	{
		MeshAsyncLoadHandle.Get()->CancelHandle();
	}
}

void UBSEquipmentSkeletalMeshComponent::DestroyComponent(bool bPromoteChildren)
{
	CancelLoading();
	
	Super::DestroyComponent(bPromoteChildren);
}
