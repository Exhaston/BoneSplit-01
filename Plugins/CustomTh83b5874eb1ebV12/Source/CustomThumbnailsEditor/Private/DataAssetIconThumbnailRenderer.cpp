// Copyright JB Dev. All Rights Reserved.

#include "DataAssetIconThumbnailRenderer.h"
#include <ThumbnailRendering/BlueprintThumbnailRenderer.h>

void UDataAssetIconThumbnailRenderer::Draw(UObject* Object, int32 X, int32 Y, uint32 Width, uint32 Height, FRenderTarget* RenderTarget, FCanvas* Canvas, bool bAdditionalViewFamily)
{
	UDataAsset* DataAsset = Cast<UDataAsset>(Object);

	if (DataAsset)
	{
		if (DataAsset->Implements<UIconThumbnailInterface>())
		{
			const TSoftObjectPtr<UTexture2D> TextureSoftPtr = IIconThumbnailInterface::Execute_GetIcon(DataAsset);
			UTexture2D* Texture2D = TextureSoftPtr.IsNull() ? nullptr : TextureSoftPtr.LoadSynchronous();
			FLinearColor Tint = IIconThumbnailInterface::Execute_GetTint(DataAsset);
			if (Tint.A == 0)
				Tint = FLinearColor::White;
			if (Texture2D)
			{
				FCanvasTileItem CanvasTile(FVector2D(X, Y), Texture2D->GetResource(), FVector2D(Width, Height), FLinearColor::White);
				CanvasTile.BlendMode = SE_BLEND_AlphaBlend;
				CanvasTile.SetColor(Tint);
				CanvasTile.Draw(Canvas);
				return;
			}
		}
	}
	UBlueprintThumbnailRenderer::Draw(Object, X, Y, Width, Height, RenderTarget, Canvas, bAdditionalViewFamily);
}

bool UDataAssetIconThumbnailRenderer::CanVisualizeAsset(UObject* Object)
{
	if (Super::CanVisualizeAsset(Object))
		return true;

	UDataAsset* DataAsset = Cast<UDataAsset>(Object);
	if (!DataAsset)
		return false;

	if (!DataAsset->Implements<UIconThumbnailInterface>())
		return false;

	const TSoftObjectPtr<UTexture2D> TextureSoftPtr = IIconThumbnailInterface::Execute_GetIcon(DataAsset);
	UTexture2D* Texture2D = TextureSoftPtr.IsNull() ? nullptr : TextureSoftPtr.LoadSynchronous();
	if (!Texture2D)
		return false;

	return true;
}
