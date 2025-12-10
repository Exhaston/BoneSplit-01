// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "BSGameplayEffect.generated.h"

/**
 * 
 */
UCLASS()
class BONESPLIT_API UBSGameplayEffect : public UGameplayEffect
{
	GENERATED_BODY()
	
public:
	
	UBSGameplayEffect();
	
#if WITH_EDITOR
	
	virtual bool CanEditChange(const FProperty* InProperty) const override;
	
#endif
};
