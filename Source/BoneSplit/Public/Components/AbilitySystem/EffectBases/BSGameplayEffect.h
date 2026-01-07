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
	
	//This effect will not last inbetween map loads.
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	bool bClearBetweenMaps = false;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	bool bClearOnHub = false;
	
#if WITH_EDITOR
	
	virtual bool CanEditChange(const FProperty* InProperty) const override;
	
#endif
};
