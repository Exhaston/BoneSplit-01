// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Widgets/BSWButtonBase.h"

#include "CommonTextBlock.h"

void UBSWButtonBase::NativePreConstruct()
{
	Super::NativePreConstruct();
	
	ButtonTextBlock->SetText(ButtonText);
}
