// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "CAAnimNotify_AbilityEvent.generated.h"

/**
 * 
 */
UCLASS(DisplayName="Ability Event")
class CHARACTERABILITIES_API UCAAnimNotify_AbilityEvent : public UAnimNotify
{
	GENERATED_BODY()
	
public:
	
	UCAAnimNotify_AbilityEvent();
	
#if WITH_EDITOR
	
	virtual FString GetNotifyName_Implementation() const override;
	
#endif
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta=(ClampMin=0, ClampMax=1), Category="Ability Event")
	float EventMagnitude = 1;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FGameplayTag EventTag;
	
	virtual void Notify(
		USkeletalMeshComponent* MeshComp, 
		UAnimSequenceBase* Animation, 
		const FAnimNotifyEventReference& EventReference) override;
};
