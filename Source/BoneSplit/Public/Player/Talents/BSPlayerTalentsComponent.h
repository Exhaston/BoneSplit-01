// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "Components/ActorComponent.h"
#include "BSPlayerTalentsComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class BONESPLIT_API UBSPlayerTalentsComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	
	UBSPlayerTalentsComponent();
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual void AddTag(FGameplayTag InTag);
	virtual void RemoveTag(FGameplayTag InTag);
	virtual int32 GetTagCount(FGameplayTag InTag);
	
	UPROPERTY(Replicated)
	FGameplayTagCountContainer TalentTags;
	
};
