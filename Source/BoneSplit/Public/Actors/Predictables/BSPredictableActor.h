// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BSPredictableActor.generated.h"

struct FGameplayAbilityTargetDataHandle;
class UGameplayAbility;


/*
 * Predictable Actor that should be spawned from Ability. This will spawn for the caller and authority. 
 * Authority will then further replicate it to other clients. 
 * A replicated version of this won't spawn back for the calling client.
 */
UCLASS()
class BONESPLIT_API ABSPredictableActor : public AActor
{
	GENERATED_BODY()

public:
	
	explicit ABSPredictableActor(const FObjectInitializer& ObjectInitializer);
	
	virtual bool IsNetRelevantFor(
		const AActor* RealViewer, const AActor* ViewTarget, const FVector& SrcLocation) const override;
	
	//Call before spawning, use SpawnDeferred()
	virtual void InitializePredictableActor(UGameplayAbility* InOwningAbility, 
		const FGameplayAbilityTargetDataHandle& TargetData, bool bShouldReplicate = false);
	
	//Returns null if not authority or owning client.
	UGameplayAbility* GetOwningAbility() const;

private:

	UPROPERTY()
	TWeakObjectPtr<UGameplayAbility> OwningAbility;
};
