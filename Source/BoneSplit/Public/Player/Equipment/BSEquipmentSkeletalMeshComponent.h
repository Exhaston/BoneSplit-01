// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"
#include "BSEquipmentSkeletalMeshComponent.generated.h"

struct FStreamableHandle;
DECLARE_MULTICAST_DELEGATE_OneParam(FBSOnSkeletalMeshLoaded, USkeletalMesh* SkeletalMeshAsset);

static FName ColorParamName = FName("ColorOverride");

UCLASS(DisplayName="EquipmentSkeletalMeshComp", NotBlueprintable, NotBlueprintType)
class BONESPLIT_API UBSEquipmentSkeletalMeshComponent : public USkeletalMeshComponent
{
	GENERATED_BODY()
	
public:
	
	virtual void LoadSkeletalMeshAsync(TSoftObjectPtr<USkeletalMesh> MeshAsset);
	virtual void LoadSkeletalMeshSync(TSoftObjectPtr<USkeletalMesh> MeshAsset);
	
	virtual void SetMaterialColor(const FColor& NewColor);
	
	virtual bool IsLoadingInProgress() const;
	virtual void CancelLoading();
	
	FBSOnSkeletalMeshLoaded& GetOnSkeletalMeshLoaded() { return OnSkeletalMeshLoaded; }
	
	virtual void DestroyComponent(bool bPromoteChildren = false) override;
	
protected:
	
	virtual void SetSkeletalMesh(USkeletalMesh* NewMesh, bool bReinitPose = true) override;
	
	UPROPERTY(Transient)
	TSoftObjectPtr<USkeletalMesh> CurrentMeshBeingLoaded;
	
	TSharedPtr<FStreamableHandle> MeshAsyncLoadHandle;
	
	FBSOnSkeletalMeshLoaded OnSkeletalMeshLoaded;
	
	UPROPERTY()
	FColor MaterialColor = FColor::White;
};
