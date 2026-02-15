// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "BSMathLibrary.generated.h"

/**
 * 
 */
UCLASS(DisplayName="BS Math Library")
class BONESPLIT_API UBSMathLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	
	//Spherically interpolates A towards B by T (Alpha, 0 - 1)
	UFUNCTION(BlueprintCallable, DisplayName="Slerp")
	static FVector Slerp(const FVector& A, const FVector& B, const float T);
};
