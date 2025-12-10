// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Widgets/BSLoadingScreenWidget.h"

#include "CommonRichTextBlock.h"

void UBSLoadingScreenWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
}

void UBSLoadingScreenWidget::NativeConstruct()
{
	Super::NativeConstruct();
	StatusText->SetText(LoadingCompleteText);
	StatusText->SetVisibility(ESlateVisibility::Hidden);
	
	FCoreUObjectDelegates::PostLoadMapWithWorld.AddWeakLambda(this, [this](UWorld* LoadedWorld)
	{
		StatusText->SetVisibility(ESlateVisibility::Visible);
	});
}

void UBSLoadingScreenWidget::NativeDestruct()
{
	Super::NativeDestruct();
	FCoreUObjectDelegates::PostLoadMapWithWorld.RemoveAll(this);
}

void UBSLoadingScreenWidget::LoadingComplete()
{
	
}
