// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Widgets/Base/BSLoadingScreenWidget.h"

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
	
}

void UBSLoadingScreenWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

void UBSLoadingScreenWidget::LoadingComplete()
{
	
}
