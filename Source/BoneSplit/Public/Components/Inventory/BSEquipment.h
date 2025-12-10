// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Object.h"
#include "BSEquipment.generated.h"

class UGameplayAbility;
class UGameplayEffect;
/**
 *   Class intended as a base for equipment instances. The instances will point to this, 
 *   information should only be from the CDO except dynamic changing information. This is stored on the instance.
 */
UCLASS(Blueprintable, BlueprintType, Abstract, DisplayName="Item")
class BONESPLIT_API UBSEquipment : public UObject
{
	GENERATED_BODY()
	
public:
	
#if WITH_EDITOR
	
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
	
	virtual bool CanEditChange(const FProperty* InProperty) const override;
	
	virtual void PreSave(FObjectPreSaveContext SaveContext) override;

#endif
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FText EquipmentName;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta=(Categories="Equipment"))
	FGameplayTag SlotTag;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TArray<TSubclassOf<UGameplayEffect>> GrantedEffects;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TArray<TSubclassOf<UGameplayAbility>> GrantedAbilities;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FGameplayTagContainer GrantedTags;
	
	virtual bool HasSkeletalMesh() const;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSoftObjectPtr<USkeletalMesh> SkeletalMesh;
};

USTRUCT(Blueprintable, BlueprintType, DisplayName="Equipment Instance")
struct FBSEquipmentInstance
{
	GENERATED_BODY()
	
	FBSEquipmentInstance() = default;
	
	FBSEquipmentInstance(TSubclassOf<UBSEquipment> ParentClass, int32 InItemLevel);
	
	explicit FBSEquipmentInstance(TSubclassOf<UBSEquipment> ParentClass);
	
	const UBSEquipment* GetSourceItemCDO() const
	{
		return GetDefault<UBSEquipment>(SourceItemClass);
	}
	
	TSubclassOf<UBSEquipment> GetSourceItemClass() const
	{
		return SourceItemClass;
	}
	
	UPROPERTY()
	int32 ItemLevel = 0;
	
	int32 SetItemLevel(const int32 InItemLevel, bool& ValidChange)
	{
		const int32 OldItemLevel = ItemLevel;
		ItemLevel = FMath::Clamp<int32>(InItemLevel, 0, 4);
		ValidChange = OldItemLevel != ItemLevel;
		return ItemLevel;
	}
	
	int32 GetItemLevel() const
	{
		return ItemLevel;
	}
	
	FText GetItemName() const;
	
	FText GetItemDescription() const;
	
	UPROPERTY()
	TSubclassOf<UBSEquipment> SourceItemClass;
};
