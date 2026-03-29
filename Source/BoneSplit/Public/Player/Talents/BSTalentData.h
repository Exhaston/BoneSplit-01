// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "BSTalentData.generated.h"

struct FGameplayTagContainer;
/**
 * 
 */
UCLASS(Blueprintable, BlueprintType, Const, DisplayName="BS Talent Data")
class BONESPLIT_API UBSTalentData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TSoftObjectPtr<UTexture2D> TalentIcon;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FLinearColor TalentTintColor;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta=(ClampMin=1))
	int32 MaxStacks = 1;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FGameplayTagContainer GainedTags;
};
