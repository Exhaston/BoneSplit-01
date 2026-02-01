// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "GameplayTagContainer.h"
#include "IconThumbnailInterface.h"
#include "UObject/Object.h"
#include "BSEquipment.generated.h"

class UGameplayAbility;
class UGameplayEffect;

USTRUCT(Blueprintable, BlueprintType, DisplayName="Equipment Mesh Info")
struct FBSEffectMeshInfo
{
	GENERATED_BODY()
	                                                                             
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta=(Categories="EquipmentMesh"))
	FGameplayTag SlotTag;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSoftObjectPtr<USkeletalMesh> MeshAsset;
};

UCLASS(Blueprintable, BlueprintType, Abstract, DisplayName="Equipment Defintion", HideCategories=("Duration", "Stacking"), meta=(PrioritizeCategories = "Equipment"))
class UBSEquipmentEffect : public UGameplayEffect, public IIconThumbnailInterface
{
	GENERATED_BODY()

public:
	
	UBSEquipmentEffect();
	
#if WITH_EDITOR
	
	virtual bool CanEditChange(const FProperty* InProperty) const override;
	
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
	
#endif
	
	virtual TSoftObjectPtr<UTexture2D> GetIcon_Implementation() const override;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Equipment")
	FText EquipmentName;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Equipment")
	TSoftObjectPtr<UTexture2D> Icon;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta=(Categories="Equipment"), Category="Equipment")
	FGameplayTag SlotTag;
	
	//Optional info about slots to equip meshes into. 
	//This doesn't enforce correct slots etc. because of edge cases like offhand or quivers and such.
	//Remember if defining a weapon set main hand and offhand. If no offhand wanted set to null
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Equipment")
	TArray<FBSEffectMeshInfo> MeshInfo;
};
                                                                        