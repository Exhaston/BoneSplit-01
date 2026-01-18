// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "BSTargetMode.generated.h"

/**
 * 
 */
UCLASS(DefaultToInstanced, Abstract, EditInlineNew)
class BONESPLIT_API UBSTargetMode : public UObject
{
	GENERATED_BODY()
	
public:
	
	virtual AActor* GetTarget(AActor* InAvatarActor) { return nullptr; }
	
};

UCLASS()
class BONESPLIT_API UBSTargetMode_RandomThreat : public UBSTargetMode
{
	GENERATED_BODY()
public:
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	bool bFilterByReachable = true;
	
	virtual AActor* GetTarget(AActor* InAvatarActor) override;
	
};

UCLASS()
class BONESPLIT_API UBSTargetMode_HighestThreat : public UBSTargetMode
{
	GENERATED_BODY()
public:
	
	virtual AActor* GetTarget(AActor* InAvatarActor) override;
	
};

UCLASS()
class UBSTargetMode_LowestThreat : public UBSTargetMode
{
	GENERATED_BODY()

public:
	
	virtual AActor* GetTarget(AActor* InAvatarActor) override;
	
};

UCLASS()
class UBSTargetMode_RandomNearby : public UBSTargetMode
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float Radius = 1000;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	bool bFilterByReachable = true;
	
	virtual AActor* GetTarget(AActor* InAvatarActor) override;
};

UCLASS()
class UBSTargetMode_Closest : public UBSTargetMode
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float Radius = 1000;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	bool bFilterByReachable = true;
	
	virtual AActor* GetTarget(AActor* InAvatarActor) override;
};

UCLASS()
class UBSTargetMode_Farthest : public UBSTargetMode
{
	GENERATED_BODY()

public:
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float Radius = 1000;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	bool bFilterByReachable = true;
	
	virtual AActor* GetTarget(AActor* InAvatarActor) override;
};

UCLASS()
class UBSTargetMode_ClosestThreat : public UBSTargetMode
{
	GENERATED_BODY()

public:
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	bool bFilterByReachable = true;
	
	virtual AActor* GetTarget(AActor* InAvatarActor) override;
};

UCLASS()
class UBSTargetMode_FarthestThreat : public UBSTargetMode
{
	GENERATED_BODY()

public:
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	bool bFilterByReachable = true;
	
	virtual AActor* GetTarget(AActor* InAvatarActor) override;
};



