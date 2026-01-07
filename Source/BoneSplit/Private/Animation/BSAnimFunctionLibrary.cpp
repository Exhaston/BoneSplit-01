// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Animation/BSAnimFunctionLibrary.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "BoneSplit/BoneSplit.h"
#include "Components/AbilitySystem/BSAbilityFunctionLibrary.h"
#include "Components/AbilitySystem/BSDynamicDecalComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

void UBSAnimNotify_CustomOverlap::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                         const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	if (!MeshComp || !MeshComp->GetOwner()) return;

	FTransform BoneTransform = MeshComp->GetBoneTransform(ParentBone);
	
	BoneTransform.SetRotation(BoneTransform.TransformRotation(RotationOffset.Quaternion()));

	BoneTransform.SetLocation(MeshComp->GetComponentTransform().TransformPosition(LocationOffset));
	
	// =================================================================================================================
	// Overlap Actors
	// ================================================================================================================= 

	for (auto FoundActor : 	
		UBSAbilityFunctionLibrary::SliceShapeOverlap(
		MeshComp->GetOwner(),
		BoneTransform,
		Forward,
		Up,
		Angle, 
		Height, 
		Distance,
		{ECC_Pawn}))
	{
		if (EventTag.IsValid())
		{
			if (const IAbilitySystemInterface* AscInterface = Cast<IAbilitySystemInterface>(MeshComp->GetOwner()))
			{
				FGameplayEventData Payload;
				Payload.EventTag = EventTag;
				Payload.EventMagnitude = EventWeight;
				Payload.ContextHandle = AscInterface->GetAbilitySystemComponent()->MakeEffectContext();
				Payload.Target = FoundActor;
				AscInterface->GetAbilitySystemComponent()->HandleGameplayEvent(EventTag, &Payload);
			}
		}
	}
	
	// =================================================================================================================
	// Editor only debug, might change to build as well if needed
	// ================================================================================================================= 

#if WITH_EDITOR 
	
	if (BSConsoleVariables::CVarBSDebugHitDetection.GetValueOnGameThread() 
		|| !MeshComp->GetOwner()->GetWorld()->IsGameWorld())
	{	
		UBSAbilityFunctionLibrary:: DrawDebugSlice(
			MeshComp->GetOwner(), 
			BoneTransform,
			Forward,
			Up,
			Distance,
			Height, 
			Angle, 
			12,
			GetEditorColor().ToFColor(true), 
			BSConsoleVariables::CVarBSDebugHitDetectionDuration.GetValueOnGameThread());
	}
	
#endif
}

#if WITH_EDITOR

FString UBSAnimNotify_CustomOverlap::GetNotifyName_Implementation() const
{
	return Super::GetNotifyName_Implementation();
}

#endif


void UBSNotifyState_AreaIndicator::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	
	if (!MeshComp || !MeshComp->GetOwner()) return;

	if (DecalComponents.Contains(MeshComp))
	{
		DecalComponents[MeshComp]->DestroyComponent();
		DecalComponents.Remove(MeshComp);
	}
	
	MeshComp->SetReceivesDecals(false);
	
	UBSDynamicDecalComponent* NewComp =
		DecalComponents.Add(MeshComp, NewObject<UBSDynamicDecalComponent>(MeshComp->GetOwner()));
	
	NewComp->SetupAttachment(MeshComp, ParentBone);
	NewComp->SetRelativeTransform(DecalTransform);
	NewComp->RegisterDynamicDecal(Material);
}

void UBSNotifyState_AreaIndicator::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	
	if (!MeshComp || !MeshComp->GetOwner()) return;
	
	if (DecalComponents.Contains(MeshComp))
	{
		DecalComponents[MeshComp]->DestroyComponent();
		DecalComponents.Remove(MeshComp);
	}
}

void UBSNotifyState_AreaIndicator::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);
	
	if (!MeshComp || !MeshComp->GetOwner()) return;

	if (DecalComponents.Contains(MeshComp))
	{
		const float StartTime = EventReference.GetNotify()->GetTriggerTime();
		const float EndTime   = EventReference.GetNotify()->GetEndTriggerTime();
		const float CurrentTime = EventReference.GetCurrentAnimationTime();
		const float Duration = FMath::Max(EndTime - StartTime, KINDA_SMALL_NUMBER);
		
		const float Progress = FMath::Clamp<float>(((CurrentTime - StartTime) / Duration) + ProgressOffset, 0, 1);
		DecalComponents[MeshComp]->GetDynamicMaterial()->SetScalarParameterValue("Percent", Progress);
	}
}

#if WITH_EDITOR

FString UBSANS_AllowRotation::GetNotifyName_Implementation() const
{
	return "Rotate To Target";
}

#endif

void UBSANS_AllowRotation::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                       const float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	if (!MeshComp || !MeshComp->GetOwner()) return;
	
	if (const ACharacter* Character = Cast<ACharacter>(MeshComp->GetOwner()))
	{
		OldRotationRate = Character->GetCharacterMovement()->RotationRate.Yaw;
		Character->GetCharacterMovement()->RotationRate.Yaw = NewRotationRate;
		Character->GetCharacterMovement()->bAllowPhysicsRotationDuringAnimRootMotion = true;
	}
}

void UBSANS_AllowRotation::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	if (!MeshComp || !MeshComp->GetOwner()) return;
	
	if (const ACharacter* Character = Cast<ACharacter>(MeshComp->GetOwner()))
	{
		Character->GetCharacterMovement()->RotationRate.Yaw = OldRotationRate;
		Character->GetCharacterMovement()->bAllowPhysicsRotationDuringAnimRootMotion = false;
	}
}
