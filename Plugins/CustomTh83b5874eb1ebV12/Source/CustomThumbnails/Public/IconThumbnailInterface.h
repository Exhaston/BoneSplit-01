// Copyright JB Dev. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IconThumbnailInterface.generated.h"

UINTERFACE(BlueprintType)
 class CUSTOMTHUMBNAILS_API UIconThumbnailInterface : public UInterface
 {
	GENERATED_BODY()
 };

class CUSTOMTHUMBNAILS_API IIconThumbnailInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CustomThumbnail")
		TSoftObjectPtr<UTexture2D> GetIcon() const;
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "CustomThumbnail")
		FLinearColor GetTint() const;

};
