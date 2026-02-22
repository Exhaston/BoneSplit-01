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

USTRUCT(BlueprintType)
struct FBSMeshBindInfo
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FName SocketAttachName = NAME_None;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TSoftObjectPtr<USkeletalMesh> SkeletalMesh;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, AdvancedDisplay)
	bool bFollowPose = true;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, AdvancedDisplay, meta=(EditCondition="!bFollowPose", EditConditionHides))
	TSubclassOf<UAnimInstance> AnimBP = nullptr;
};

UCLASS(Blueprintable, BlueprintType, Abstract, DisplayName="Equipment Defintion", HideCategories=("Duration", "Stacking"), meta=(PrioritizeCategories = "Equipment"))
class UBSEquipmentEffect : public UGameplayEffect, public IIconThumbnailInterface
{
	GENERATED_BODY()

public:
	
	UBSEquipmentEffect();
	
#if WITH_EDITOR
	
	virtual void PostLoad() override;
	
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	
	virtual bool CanEditChange(const FProperty* InProperty) const override;
	
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
	
#endif
	
	virtual TSoftObjectPtr<UTexture2D> GetIcon_Implementation() const override;
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category="Data", meta=(IgnoreForMemberInitializationTest))  
	FGuid EquipmentID = FGuid::NewGuid();
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Equipment")
	FText EquipmentName;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Equipment")
	TSoftObjectPtr<UTexture2D> Icon;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta=(Categories="Equipment"), Category="Equipment")
	FGameplayTag SlotTag;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta=(Categories="EquipmentMesh", ForceInlineRow))
	TArray<FBSMeshBindInfo> EquipmentMeshes;
};
                                                                        