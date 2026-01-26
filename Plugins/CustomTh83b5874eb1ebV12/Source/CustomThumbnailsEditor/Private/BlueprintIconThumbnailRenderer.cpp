// Copyright JB Dev. All Rights Reserved.


#include "BlueprintIconThumbnailRenderer.h"

void UBlueprintIconThumbnailRenderer::Draw(UObject* Object, int32 X, int32 Y, uint32 Width, uint32 Height, FRenderTarget* RenderTarget, FCanvas* Canvas, bool bAdditionalViewFamily)
{
	UBlueprint* Blueprint = Cast<UBlueprint>(Object);
	if (Blueprint)
	{
		UObject *DefaultObject = Blueprint->GeneratedClass->GetDefaultObject();
		if (DefaultObject->Implements<UIconThumbnailInterface>())
		{
			const TSoftObjectPtr<UTexture2D> TextureSoftPtr = IIconThumbnailInterface::Execute_GetIcon(DefaultObject);
			UTexture2D* Texture2D = TextureSoftPtr.IsNull() ? nullptr : TextureSoftPtr.LoadSynchronous();
			FLinearColor Tint = IIconThumbnailInterface::Execute_GetTint(DefaultObject);
			if (Tint.A == 0)
				Tint = FLinearColor::White;
			if (Texture2D && Texture2D->GetResource())
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

bool UBlueprintIconThumbnailRenderer::CanVisualizeAsset(UObject* Object)
{
	if (Super::CanVisualizeAsset(Object))
		return true;

	UBlueprint* Blueprint = Cast<UBlueprint>(Object);
	if (!Blueprint)
		return false;

	UObject *DefaultObject = Blueprint->GeneratedClass ? Blueprint->GeneratedClass->GetDefaultObject() : nullptr;
	if (!DefaultObject)
		return false;

	if (!DefaultObject->Implements<UIconThumbnailInterface>())
		return false;

	const TSoftObjectPtr<UTexture2D> TextureSoftPtr = IIconThumbnailInterface::Execute_GetIcon(DefaultObject);
	UTexture2D* Texture2D = TextureSoftPtr.IsNull() ? nullptr : TextureSoftPtr.LoadSynchronous();
	if (!Texture2D)
		return false;

	return true;
}
