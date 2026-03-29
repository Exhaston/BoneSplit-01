// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "BSPowerUpPool.generated.h"

class UBSPowerUpData;

USTRUCT(BlueprintType)
struct FBSPowerUpRow
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TObjectPtr<UBSPowerUpData> PowerDataAsset;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta=(ClampMin=1))
	int32 MaxCount = 10;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta=(Units=Percent, ClampMin=0.1f, ClampMax=100))
	float Weight = 25.f;
	
	bool operator==(const FBSPowerUpRow& Other) const
	{
		return PowerDataAsset == Other.PowerDataAsset;
	}
	
};

FORCEINLINE uint32 GetTypeHash(const FBSPowerUpRow& Row)
{
	return GetTypeHash(Row.PowerDataAsset);
}

UCLASS(Const, Blueprintable, BlueprintType)
class BONESPLIT_API UBSPowerUpPool : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TSet<FBSPowerUpRow> PowerUps;
};
