// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "IconThumbnailInterface.h"
#include "BSBuffEffect.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType, DisplayName="Buff Effect")
class BONESPLIT_API UBSBuffEffect : public UGameplayEffect, public IIconThumbnailInterface
{
	GENERATED_BODY()
	
public:     
	
	UBSBuffEffect();
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Buff")
	TSoftObjectPtr<UTexture2D> BuffIcon;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Buff")
	FLinearColor BuffIconTint = FLinearColor::White;
	
	virtual TSoftObjectPtr<UTexture2D> GetIcon_Implementation() const override;
	virtual FLinearColor GetTint_Implementation() const override;
};
