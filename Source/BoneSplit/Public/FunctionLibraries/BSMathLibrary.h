// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "BSMathLibrary.generated.h"

/**
 * 
 */
UCLASS()
class BONESPLIT_API UBSMathLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
	public:
	
	
	
};

namespace BSMathFunctions
{
	
	template<typename KeyType>
	bool GetWeightedRandomKey(const TMap<KeyType, float>& Map,KeyType& OutKey)
	{
		float TotalWeight = 0.f;

		for (const auto& Pair : Map)
		{
			if (Pair.Value > 0.f)
			{
				TotalWeight += Pair.Value;
			}
		}

		if (TotalWeight <= 0.f)
		{
			return false; // no valid weights
		}

		float RandomValue = FMath::FRandRange(0.f, TotalWeight);

		for (const auto& Pair : Map)
		{
			if (Pair.Value <= 0.f)
			{
				continue;
			}

			RandomValue -= Pair.Value;

			if (RandomValue <= 0.f)
			{
				OutKey = Pair.Key;
				return true;
			}
		}

		return false;
	}
	
}
