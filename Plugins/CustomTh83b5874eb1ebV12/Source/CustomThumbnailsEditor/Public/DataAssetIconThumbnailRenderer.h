// Copyright JB Dev. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "IconThumbnailInterface.h"
#include "ThumbnailRendering/BlueprintThumbnailRenderer.h"
#include "CanvasItem.h"
#include "DataAssetIconThumbnailRenderer.generated.h"

class FCanvas;
class FRenderTarget;

UCLASS()
class CUSTOMTHUMBNAILSEDITOR_API UDataAssetIconThumbnailRenderer : public UBlueprintThumbnailRenderer
{
	GENERATED_BODY()

	virtual void Draw(UObject* Object, int32 X, int32 Y, uint32 Width, uint32 Height, FRenderTarget* RenderTarget, FCanvas* Canvas, bool bAdditionalViewFamily) override;
	virtual bool CanVisualizeAsset(UObject* Object) override;

};