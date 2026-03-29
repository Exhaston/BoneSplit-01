// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "AnimInstance/MMobAnimInstance.h"
#include "BSPlayerAnimInstance.generated.h"


/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class BONESPLIT_API UBSPlayerAnimInstance : public UMMobAnimInstance
{
	GENERATED_BODY()
	
public:
	
	UFUNCTION(BlueprintNativeEvent, Category="Player", DisplayName="OnEquipmentAdded")
	void BP_OnEquipmentAdded(UStaticMesh* MeshAsset);
	                                                    
	UFUNCTION(BlueprintNativeEvent, Category="Player", DisplayName="OnEquipmentRemoved")
	void BP_OnEquipmentRemoved(UStaticMesh* MeshAsset);
	
	virtual void NativeOnTagAdded(const FGameplayTag InTag)
	{
		BP_OnGameplayTagAdded(InTag);
		OwnedTagsContainer.AddTag(InTag);
	}
	
	virtual void NativeOnTagRemoved(const FGameplayTag InTag)
	{
		BP_OnGameplayTagRemoved(InTag);
		OwnedTagsContainer.RemoveTag(InTag);
	};
	
	UFUNCTION(BlueprintNativeEvent, Category="Player", DisplayName="OnGameplayTagRemoved")
	void BP_OnGameplayTagAdded(FGameplayTag AddedTag);
	
	UFUNCTION(BlueprintNativeEvent, Category="Player", DisplayName="OnGameplayTagRemoved")
	void BP_OnGameplayTagRemoved(FGameplayTag RemovedTag);
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Player")
	FGameplayTagContainer OwnedTagsContainer;
};
