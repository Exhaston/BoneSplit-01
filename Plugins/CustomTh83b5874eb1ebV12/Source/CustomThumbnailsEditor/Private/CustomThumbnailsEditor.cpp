// Copyright JB Dev. All Rights Reserved.

#include "CustomThumbnailsEditor.h"
#include "ThumbnailRendering/ThumbnailManager.h"
#include "BlueprintIconThumbnailRenderer.h"
#include "DataAssetIconThumbnailRenderer.h"

#define LOCTEXT_NAMESPACE "FCustomThumbnailsEditorModule"

void FCustomThumbnailsEditorModule::StartupModule()
{
	UThumbnailManager::Get().UnregisterCustomRenderer(UBlueprint::StaticClass());
	UThumbnailManager::Get().RegisterCustomRenderer(UBlueprint::StaticClass(), UBlueprintIconThumbnailRenderer::StaticClass());
	UThumbnailManager::Get().UnregisterCustomRenderer(UDataAsset::StaticClass());
	UThumbnailManager::Get().RegisterCustomRenderer(UDataAsset::StaticClass(), UDataAssetIconThumbnailRenderer::StaticClass());
}

void FCustomThumbnailsEditorModule::ShutdownModule()
{}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FCustomThumbnailsEditorModule, CustomThumbnailsEditor)