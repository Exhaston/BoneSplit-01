// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "BSMobSubsystem.generated.h"

class AAIController;
class ABSMobBase;

USTRUCT(BlueprintType)
struct FBSTargetGroup
{
	GENERATED_BODY()
	
	FBSTargetGroup() = default;
	
	UPROPERTY()
	TWeakObjectPtr<AActor> Target;

	UPROPERTY()
	TArray<TWeakObjectPtr<AAIController>> AIs;
};

UCLASS()
class BONESPLIT_API UBSMobSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()
	
public:
	
	virtual void Tick(float DeltaTime) override;
	
	//Required for tick
	virtual TStatId GetStatId() const override;
	
	//Use when assigning a target. Will update movement and focus automatically.
	UFUNCTION(BlueprintCallable)
	void RegisterTarget(AAIController* AIController, AActor* Target);
	
	UFUNCTION(BlueprintCallable)
	void UnRegisterTarget(AAIController* AIController);
	
	//Returns a completely random player
	virtual AActor* GetRandomPlayer();
	
	//Returns a player from the player list, then increments. This ensures an even spread to all players. 
	//May return nullptr if selected PlayerState has no pawn
	virtual AActor* GetPlayerDeterministic();
	
private:
	
	UPROPERTY()
	int32 CurrentPlayerIncrement;
	
	UPROPERTY()
	TArray<FBSTargetGroup> TargetGroups;

	//Update time for the AI pathfinding.
	UPROPERTY()
	float UpdateInterval = 0.5;
	
	UPROPERTY()
	float TimeAccumulator = 0;

	FBSTargetGroup* FindGroup(AActor* Player);
};
