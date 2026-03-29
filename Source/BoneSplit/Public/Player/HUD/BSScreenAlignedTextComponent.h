// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/TextRenderComponent.h"
#include "BSScreenAlignedTextComponent.generated.h"


UCLASS()
class BONESPLIT_API UBSScreenAlignedTextComponent : public UTextRenderComponent
{
	GENERATED_BODY()

public:
	
	UBSScreenAlignedTextComponent();
	
# if WITH_EDITOR
	
	virtual bool CanEditChange(const FProperty* InProperty) const override;
	
#endif
	
};
