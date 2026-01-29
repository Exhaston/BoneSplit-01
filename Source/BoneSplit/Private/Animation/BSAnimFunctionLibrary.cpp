// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Animation/BSAnimFunctionLibrary.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "BoneSplit/BoneSplit.h"
#include "Components/AbilitySystem/BSAbilityLibrary.h"
#include "Components/AbilitySystem/BSDynamicDecalComponent.h"
#include "Components/AbilitySystem/BSShapeLibrary.h"
#include "Engine/OverlapResult.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

#define ENSURE_OWNER \
if (!MeshComp || !MeshComp->GetOwner()) return;

void FBSAnimEventPayloadData::SendPayload(
UAbilitySystemComponent* SourceAsc, AActor* TargetActor) const
{
	FGameplayEventData Payload;
	Payload.EventTag = EventPayloadTag;
	Payload.EventMagnitude = EventMagnitude;
	Payload.ContextHandle = SourceAsc->MakeEffectContext();
	Payload.Target = TargetActor;

	const FVector MeshSpaceOffset = {KnockbackDirection.Y, KnockbackDirection.X, KnockbackDirection.Z};
	const FVector WorldSpaceOffset = SourceAsc->GetAvatarActor()->GetActorTransform().TransformVector(MeshSpaceOffset);

	const FVector Origin = TargetActor->GetActorLocation() + WorldSpaceOffset;
	
	Payload.ContextHandle.AddOrigin(Origin);
	
#if WITH_EDITOR
	
	if (BS_HIT_DEBUG)
	{
		DrawDebugPoint(
			SourceAsc->GetAvatarActor()->GetWorld(),
			TargetActor->GetActorLocation(),
			25, 
			FColor::Red,
			false,
			BSConsoleVariables::CVarBSDebugHitDetectionDuration.GetValueOnGameThread());
	}
	
#endif
	
	SourceAsc->HandleGameplayEvent(EventPayloadTag, &Payload);
}

UBSAnimNotify_WedgeOverlap::UBSAnimNotify_WedgeOverlap()
{
#if WITH_EDITORONLY_DATA
	NotifyColor = FColor::Red;
#endif
}

void UBSAnimNotify_WedgeOverlap::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                        const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	ENSURE_OWNER

	UWorld* World = GEngine->GetWorldFromContextObjectChecked(MeshComp->GetOwner());

	FTransform BoneTransform = MeshComp->GetBoneTransform(ParentBone);
	
	BoneTransform.SetRotation(BoneTransform.TransformRotation(RotationOffset.Quaternion()));

	BoneTransform.SetLocation(MeshComp->GetComponentTransform().TransformPosition(LocationOffset));
	
	BoneTransform.SetScale3D(MeshComp->GetComponentScale()); //No weird bone transforms, stick to reliable mesh axis.
	
	// =================================================================================================================
	// Overlap Actors
	// ================================================================================================================= 

	TArray<AActor*> OverlappedActors = 
		UBSShapeLibrary::OverlapShapeWedge(
			World, 
			BoneTransform, 
			Distance, 
			Height, 
			Angle, 
			{ ECC_Pawn },
			true);


	if (const IAbilitySystemInterface* AscInterface = Cast<IAbilitySystemInterface>(MeshComp->GetOwner()))
	{
		for (const auto OverlappedActor : OverlappedActors)
		{
			EventPayloadData.SendPayload(AscInterface->GetAbilitySystemComponent(), OverlappedActor);
		}
	}
	
	// =================================================================================================================
	// Editor only debug, might change to build as well if needed
	// ================================================================================================================= 

#if WITH_EDITOR 
	
	if (BS_HIT_DEBUG || !World->IsGameWorld())
	{	
		UBSShapeLibrary::DebugDrawWedge(
			World, 
			BoneTransform, 
			Distance, 
			Height, 
			Angle, 
			BSConsoleVariables::CVarBSDebugHitDetectionDuration.GetValueOnGameThread(), 
			GetEditorColor().ToFColor(true), 
			true);
	}
	
#endif
}

#if WITH_EDITOR
FString UBSAnimNotify_WedgeOverlap::GetNotifyName_Implementation() const
{
	return "Wedge Overlap";
}
#endif

UBSAnimNotify_SphereOverlap::UBSAnimNotify_SphereOverlap()
{
#if WITH_EDITORONLY_DATA
	NotifyColor = FColor::Red;
#endif
}

void UBSAnimNotify_SphereOverlap::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	ENSURE_OWNER

	const UWorld* World = GEngine->GetWorldFromContextObjectChecked(MeshComp->GetOwner());

	const FVector Positon = MeshComp->GetBoneTransform(ParentBone).TransformPosition(LocationOffset);
	const FCollisionShape SphereShape = FCollisionShape::MakeSphere(Radius);
	
	FCollisionObjectQueryParams QueryParams;
	QueryParams.AddObjectTypesToQuery(ECC_Pawn);

	TArray<AActor*> UniqueHitActors;
	
	TArray<FOverlapResult> Overlaps;
	World->OverlapMultiByObjectType(
		Overlaps, Positon, FQuat::Identity, QueryParams, SphereShape);
	
	for (const FOverlapResult& Overlap : Overlaps)
	{
		if (AActor* TargetActor = Overlap.GetActor())
		{
			UniqueHitActors.AddUnique(TargetActor);
		}
	}

	for (const auto UniqueHitActor : UniqueHitActors)
	{
		if (const IAbilitySystemInterface* AscInterface = Cast<IAbilitySystemInterface>(MeshComp->GetOwner()))
		{
			PayloadData.SendPayload(AscInterface->GetAbilitySystemComponent(), UniqueHitActor);
		}
	}
	
#if WITH_EDITOR 
	
if (BS_HIT_DEBUG || !World->IsGameWorld())
{
	DrawDebugSphere(
		World, 
		Positon,
		Radius,
		16,
		GetEditorColor().ToFColor(true),
		false, 
		BSConsoleVariables::CVarBSDebugHitDetectionDuration.GetValueOnGameThread());
}
	
#endif
}

#if WITH_EDITOR
FString UBSAnimNotify_SphereOverlap::GetNotifyName_Implementation() const
{
	return "Sphere Overlap";
}

FString UBSNotifyState_AreaIndicator::GetNotifyName_Implementation() const
{
	return "Project Effect Zone";
}
#endif

UBSNotifyState_AreaIndicator::UBSNotifyState_AreaIndicator()
{
#if WITH_EDITORONLY_DATA
	NotifyColor = FColor::Yellow;
#endif
}


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

UBSANS_AllowRotation::UBSANS_AllowRotation()
{
#if WITH_EDITOR
	NotifyColor = FColor::Green;
#endif
}

void UBSANS_AllowRotation::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                       const float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	ENSURE_OWNER
	
	if (ACharacter* Character = Cast<ACharacter>(MeshComp->GetOwner()))
	{
		bOldRotationMode = Character->GetCharacterMovement()->bOrientRotationToMovement;
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
	
	if (const ACharacter* Character = Cast<ACharacter>(MeshComp->GetOwner()))
	{
		Character->GetCharacterMovement()->bUseControllerDesiredRotation = !bOldRotationMode;
		Character->GetCharacterMovement()->bOrientRotationToMovement = bOldRotationMode;
		Character->GetCharacterMovement()->RotationRate.Yaw = OldRotationRate;
		Character->GetCharacterMovement()->bAllowPhysicsRotationDuringAnimRootMotion = false;
	}
}

void UBSANS_AllowRotation::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	ENSURE_OWNER
	
	UWorld* World = GEngine->GetWorldFromContextObjectChecked(MeshComp->GetOwner());
	
#if WITH_EDITOR
	
	if (!World->IsGameWorld())
	{
		DrawDebugDirectionalArrow(
			World, 
			MeshComp->GetComponentLocation(), 
			MeshComp->GetComponentLocation() + MeshComp->GetRightVector() * 50, 
			128, GetEditorColor().ToFColor(true), 
			false, 
			-1, 
			0, 
			2);
	}
	
#endif
	
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

