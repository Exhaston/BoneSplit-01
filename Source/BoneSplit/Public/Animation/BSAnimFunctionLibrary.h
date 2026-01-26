// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "BoneSplit/BoneSplit.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "BSAnimFunctionLibrary.generated.h"

class UAbilitySystemComponent;
struct FGameplayEventData;
class UBSDynamicDecalComponent;

USTRUCT(BlueprintType, Blueprintable, DisplayName="PayloadData")
struct FBSAnimEventPayloadData
{
	GENERATED_BODY()
	
	FBSAnimEventPayloadData() = default;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta=(Categories="AnimEvent"))
	FGameplayTag EventPayloadTag = BSTags::AnimEvent_Damage_01;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta=(ClampMin=0, ClampMax=1))
	float EventMagnitude = 1;
	
	//This will be the origin of knockbacks etc. This will be from the axis of the source actor
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta=(ClampMin=0, ClampMax=1))
	FVector KnockbackDirection = { 0, -1, -1 };
	
	void SendPayload(UAbilitySystemComponent* SourceAsc, AActor* TargetActor) const;
};
/**
 * 
 */
UCLASS()
class BONESPLIT_API UBSAnimFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
};                            

UCLASS(DisplayName="Wedge Overlap Actors")
class UBSAnimNotify_WedgeOverlap : public UAnimNotify
{
	GENERATED_BODY()

public:
	
	UBSAnimNotify_WedgeOverlap();
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Ability Event", meta=(ShowOnlyInnerProperties=true))
	FBSAnimEventPayloadData EventPayloadData;
	
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

#if WITH_EDITOR
	
	virtual FString GetNotifyName_Implementation() const override;
	
#endif
};

UCLASS(DisplayName="Sphere Overlap Actors")
class UBSAnimNotify_SphereOverlap : public UAnimNotify
{
	GENERATED_BODY()
							   
public:
	
	UBSAnimNotify_SphereOverlap();
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Ability Event", meta=(ShowOnlyInnerProperties))
	FBSAnimEventPayloadData PayloadData;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta=(AnimNotifyBoneName=true), Category="Shape")
	FName ParentBone;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta=(Units=cm), Category="Shape")
	FVector LocationOffset;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta=(Units=cm, ClampMin=1), Category="Shape")
	float Radius = 100;
	
	virtual void Notify(
		USkeletalMeshComponent* MeshComp, 
		UAnimSequenceBase* Animation, 
		const FAnimNotifyEventReference& EventReference) override;

#if WITH_EDITOR
	
	virtual FString GetNotifyName_Implementation() const override;
	
#endif
};

UCLASS(DisplayName="Project Effect Zone")
class UBSNotifyState_AreaIndicator : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	
	UBSNotifyState_AreaIndicator();
	
#if WITH_EDITOR

	virtual FString GetNotifyName_Implementation() const override;
	
#endif
	
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

UCLASS(DisplayName="Allow Rotation")
class UBSANS_AllowRotation : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	
	UBSANS_AllowRotation();
	
#if WITH_EDITOR
	
	virtual FString GetNotifyName_Implementation() const override;
	
#endif
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float NewRotationRate = 1000;
	
	UPROPERTY()
	bool bOldRotationMode = true;
	
	UPROPERTY()
	float OldRotationRate;
	
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;
	
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
