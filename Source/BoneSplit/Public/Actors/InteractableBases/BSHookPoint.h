// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BSHookPoint.generated.h"

class UWidgetComponent;

UCLASS(DisplayName="Hook Point Base", Abstract, BlueprintType, Blueprintable)
class BONESPLIT_API ABSHookPoint : public AActor
{
	GENERATED_BODY()

public:
	
	ABSHookPoint();
	
	virtual FVector GetTargetLocation(AActor* RequestedBy = nullptr) const override;
	
protected:
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	UStaticMeshComponent* HookMesh;
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	USceneComponent* EndPoint;
	
};
