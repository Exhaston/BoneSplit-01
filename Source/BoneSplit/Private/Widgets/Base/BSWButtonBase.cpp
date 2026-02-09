// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Widgets/Base/BSWButtonBase.h"

#include "CommonTextBlock.h"

void UBSWButtonBase::NativePreConstruct()
{
	Super::NativePreConstruct();
	
	ButtonTextBlock->SetText(ButtonText);
}
