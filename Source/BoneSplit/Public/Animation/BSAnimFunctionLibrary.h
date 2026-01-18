// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "BoneSplit/BoneSplit.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "BSAnimFunctionLibrary.generated.h"

class UBSDynamicDecalComponent;
/**
 * 
 */
UCLASS()
class BONESPLIT_API UBSAnimFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
};

UCLASS(DisplayName="Slice Overlap Actors")
class UBSAnimNotify_CustomOverlap : public UAnimNotify
{
	GENERATED_BODY()

public:
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Ability Event", meta=(Categories="AnimEvent"))
	FGameplayTag EventTag;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta=(ClampMin=0, ClampMax=1), Category="Ability Event")
	float EventWeight = 1;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta=(AnimNotifyBoneName=true), Category="Shape")
	FName ParentBone;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta=(Units=cm), Category="Shape")
	FVector LocationOffset;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta=(ClampMin=-180, ClampMax=180), Category="Shape")
	FRotator RotationOffset;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta=(Units=cm, ClampMin=1), Category="Shape")
	float Height = 100;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta=(Units=cm, ClampMin=1), Category="Shape")
	float Distance = 200;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta=(Units=degrees, ClampMin=1, ClampMax=360), Category="Shape")
	float Angle = 90;
	
	virtual void Notify(
		USkeletalMeshComponent* MeshComp, 
		UAnimSequenceBase* Animation, 
		const FAnimNotifyEventReference& EventReference) override;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, AdvancedDisplay, Category="Shape")
	TEnumAsByte<EAxis::Type> Forward = EAxis::Type::Y;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, AdvancedDisplay, Category="Shape")
	TEnumAsByte<EAxis::Type> Up = EAxis::Type::Z;

#if WITH_EDITOR
	
	virtual FString GetNotifyName_Implementation() const override;
	
#endif
};

UCLASS()
class UBSNotifyState_AreaIndicator : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	
	virtual void NotifyBegin(
		USkeletalMeshComponent* MeshComp, 
		UAnimSequenceBase* Animation, 
		float TotalDuration, 
		const FAnimNotifyEventReference& EventReference) override;
	
	virtual void NotifyEnd(
		USkeletalMeshComponent* MeshComp, 
		UAnimSequenceBase* Animation, 
		const FAnimNotifyEventReference& EventReference) override;
	
	virtual void NotifyTick(
		USkeletalMeshComponent* MeshComp,
		UAnimSequenceBase* Animation, 
		float FrameDeltaTime, 
		const FAnimNotifyEventReference& EventReference) override;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta=(AnimNotifyBoneName=true))
	FName ParentBone;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FTransform DecalTransform;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float ProgressOffset = 0;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	UMaterialInterface* Material;
	
	UPROPERTY()
	TMap<USkeletalMeshComponent*, UBSDynamicDecalComponent*> DecalComponents;
};

UCLASS(DisplayName="AllowRotation")
class UBSANS_AllowRotation : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	
#if WITH_EDITOR
	
	virtual FString GetNotifyName_Implementation() const override;
	
#endif
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float NewRotationRate = 1000;
	
	UPROPERTY()
	float OldRotationRate;
	
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	
};

UCLASS(DisplayName="Ability System Event")
class UBSAN_SendEventToAsc : public UAnimNotify
{
	GENERATED_BODY()

public:
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta=(Categories="AnimEvent"))
	FGameplayTag EventTag = BSTags::AnimEvent_StoreCombo;
	
#if WITH_EDITOR
	
	virtual FString GetNotifyName_Implementation() const override;
	
#endif
	
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	
};
