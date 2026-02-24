// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BSAggroComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBSOnTargetFound, AActor*, Target);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class BONESPLIT_API UBSAggroComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	
	UBSAggroComponent();
	
	UPROPERTY(BlueprintAssignable)
	FBSOnTargetFound OnTargetFoundDelegate;
	
	virtual void CheckAggroSphere(bool bCheckVisibility = true);
	
	virtual void TickComponent(
		float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	
	UPROPERTY()
	TArray<TWeakObjectPtr<AActor>> FoundActors;
	
	//If set to true, will line trace to the overlapped
	//actor to check if there is static geometry in between the actor and the owner
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	bool bVisibilityCheck;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta=(Units=Centimeters, ClampMin=0))
	float AggroRadius = 500;
};
