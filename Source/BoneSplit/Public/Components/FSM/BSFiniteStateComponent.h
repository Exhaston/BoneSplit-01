// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BSFiniteStateComponent.generated.h"


class UBSFiniteState;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class BONESPLIT_API UBSFiniteStateComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	
	explicit UBSFiniteStateComponent(const FObjectInitializer& ObjectInitializer);
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	UFUNCTION(BlueprintCallable, Category="FSM")
	void ChangeState(TSubclassOf<UBSFiniteState> NewState);
	
	UPROPERTY()
	TArray<UBSFiniteState*> PooledStates;
	
	UPROPERTY()
	UBSFiniteState* CurrentState;
};
