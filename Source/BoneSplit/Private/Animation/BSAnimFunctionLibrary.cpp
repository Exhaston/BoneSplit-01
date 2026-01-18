// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Animation/BSAnimFunctionLibrary.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "BoneSplit/BoneSplit.h"
#include "Components/AbilitySystem/BSAbilityFunctionLibrary.h"
#include "Components/AbilitySystem/BSDynamicDecalComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

#define ENSURE_OWNER \
if (!MeshComp || !MeshComp->GetOwner()) return;

void UBSAnimNotify_CustomOverlap::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                         const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	ENSURE_OWNER

	FTransform BoneTransform = MeshComp->GetBoneTransform(ParentBone);
	
	BoneTransform.SetRotation(BoneTransform.TransformRotation(RotationOffset.Quaternion()));

	BoneTransform.SetLocation(MeshComp->GetComponentTransform().TransformPosition(LocationOffset));
	
	BoneTransform.SetScale3D(MeshComp->GetComponentScale()); //No weird bone transforms, stick to reliable mesh axis.
	
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
	return "Slice Overlap";
}

#endif


void UBSNotifyState_AreaIndicator::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	
	ENSURE_OWNER

	if (DecalComponents.Contains(MeshComp))
	{
		DecalComponents[MeshComp]->DestroyComponent();
		DecalComponents.Remove(MeshComp);
	}
	
	MeshComp->SetReceivesDecals(false);
	
	UBSDynamicDecalComponent* NewComp =
		DecalComponents.Add(MeshComp, NewObject<UBSDynamicDecalComponent>(MeshComp->GetOwner()));
	
	NewComp->SetupAttachment(MeshComp, ParentBone);
	NewComp->DecalSize = {100, 100, 100};
	NewComp->SetRelativeTransform(DecalTransform);
	NewComp->RegisterDynamicDecal(Material);
}

void UBSNotifyState_AreaIndicator::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	
	ENSURE_OWNER
	
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
	
	ENSURE_OWNER

	if (DecalComponents.Contains(MeshComp))
	{
		const float StartTime = EventReference.GetNotify()->GetTriggerTime();
		const float EndTime   = EventReference.GetNotify()->GetEndTriggerTime();
		const float CurrentTime = EventReference.GetCurrentAnimationTime();
		const float Duration = FMath::Max(EndTime - StartTime, KINDA_SMALL_NUMBER);
		
		const float Progress = FMath::Clamp<float>(((CurrentTime - StartTime) / Duration) + ProgressOffset, 0, 1);
		DecalComponents[MeshComp]->GetDynamicMaterial()->SetScalarParameterValue("Percent", Progress);
		
#if WITH_EDITOR
		if (const UWorld* World = GEngine->GetWorldFromContextObjectChecked(MeshComp->GetOwner()); 
			!World->IsGameWorld())
		{
			DrawDebugBox(
				World, 
				DecalComponents[MeshComp]->GetComponentLocation(), 
				DecalComponents[MeshComp]->GetRelativeScale3D() * DecalComponents[MeshComp]->DecalSize, 
				NotifyColor, 
				false, 
				-1);
		}
#endif
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
	ENSURE_OWNER
	
	if (ACharacter* Character = Cast<ACharacter>(MeshComp->GetOwner()); Character && Character->HasAuthority())
	{
		Character->GetCharacterMovement()->bUseControllerDesiredRotation = true;
		Character->GetCharacterMovement()->bOrientRotationToMovement = false;
		OldRotationRate = Character->GetCharacterMovement()->RotationRate.Yaw;
		Character->GetCharacterMovement()->RotationRate.Yaw = NewRotationRate;
		Character->GetCharacterMovement()->bAllowPhysicsRotationDuringAnimRootMotion = true;
	}
}

void UBSANS_AllowRotation::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	ENSURE_OWNER
	
	if (const ACharacter* Character = Cast<ACharacter>(MeshComp->GetOwner()); Character && Character->HasAuthority())
	{
		Character->GetCharacterMovement()->bUseControllerDesiredRotation = false;
		Character->GetCharacterMovement()->bOrientRotationToMovement = true;
		Character->GetCharacterMovement()->RotationRate.Yaw = OldRotationRate;
		Character->GetCharacterMovement()->bAllowPhysicsRotationDuringAnimRootMotion = false;
	}
}

#if WITH_EDITOR

FString UBSAN_SendEventToAsc::GetNotifyName_Implementation() const
{
	return "Event: " + EventTag.GetTagName().ToString();
}

#endif

void UBSAN_SendEventToAsc::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	
	ENSURE_OWNER
	
	if (!MeshComp->GetOwner()->HasLocalNetOwner() && !MeshComp->GetOwner()->HasAuthority()) return; //No viable ability owner.
	
	if (const IAbilitySystemInterface* AscInterface = Cast<IAbilitySystemInterface>(MeshComp->GetOwner()))
	{
		FGameplayEventData Payload;
		Payload.EventTag = EventTag;
		Payload.ContextHandle = AscInterface->GetAbilitySystemComponent()->MakeEffectContext();
		AscInterface->GetAbilitySystemComponent()->HandleGameplayEvent(EventTag, &Payload);
	}
}

