// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "IconThumbnailInterface.h"
#include "Engine/DataAsset.h"
#include "BSEquipmentAsset.generated.h"

struct FGameplayTag;
class UGameplayAbility;
class UGameplayEffect;

USTRUCT(BlueprintType)
struct FBSUberBoneData
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	int32 ChildBoneIndex = -1;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	int32 ParentBoneIndex = -1;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FVector BoneOffset = FVector::ZeroVector;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FRotator BoneRotationOffset = FRotator::ZeroRotator;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta=(ClampMin=0))
	float SpringStrength = 1;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta=(ClampMin=0))
	float SpringStiffness = 1;
};

USTRUCT(BlueprintType, Blueprintable, DisplayName="Equipment Mesh Data")
struct FBSEquipmentMeshData
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TSoftObjectPtr<USkeletalMesh> SkeletalMesh;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FName SocketAttachName = NAME_None;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	bool bFollowPose = true;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta=(EditCondition="!bFollowPose", EditConditionHides))
	TSubclassOf<UAnimInstance> AnimBP = nullptr;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TArray<FBSUberBoneData> BoneData;
	
	bool operator==(const FBSEquipmentMeshData& B)
	{
		return SkeletalMesh == B.SkeletalMesh;
	}
};

UENUM(BlueprintType)
enum EBSEquipmentSlot
{
	EES_Head = 0 UMETA(DisplayName="Head"),
	EES_Chest = 1 UMETA(DisplayName="Chest"),
	EES_Legs = 2 UMETA(DisplayName="Legs"),
	EES_Arms = 3 UMETA(DisplayName="Arms"),
	EES_Soul = 4 UMETA(DisplayName="Soul"),                       
	EES_Curio = 5 UMETA(DisplayName="Curio"),
	EES_Trinket = 5 UMETA(DisplayName="Trinket"),
};

UCLASS(Blueprintable, BlueprintType, Const, DisplayName="Equipment Definition Asset")
class BONESPLIT_API UBSEquipmentAsset : public UPrimaryDataAsset, public IIconThumbnailInterface
{
	GENERATED_BODY()
	
public:
	
	virtual TSoftObjectPtr<UTexture2D> GetIcon_Implementation() const override;
	
	virtual FLinearColor GetTint_Implementation() const override;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSoftObjectPtr<UTexture2D> EquipmentIcon;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FLinearColor IconTint = FLinearColor::White;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FText EquipmentName = FText::FromString("Nameless");
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FText EquipmentDescription = FText::FromString("Description");
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TEnumAsByte<EBSEquipmentSlot> Slot;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<TSubclassOf<UGameplayAbility>> GrantedAbilities;
	     
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<TSubclassOf<UGameplayEffect>> AppliedEffects;
	
	//Meshes to apply when using this equipment.
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<FBSEquipmentMeshData> MeshData;
};
