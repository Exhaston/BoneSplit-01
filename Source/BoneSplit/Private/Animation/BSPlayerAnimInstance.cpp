// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Animation/BSPlayerAnimInstance.h"

#include "Actors/Player/BSPlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

void UBSPlayerAnimInstance::InitializeAbilitySystemComponent(UAbilitySystemComponent* InAbilitySystemComponent)
{
	PlayerCharacterOwner = Cast<ABSPlayerCharacter>(TryGetPawnOwner());
	if (PlayerCharacterOwner)
	{
		Super::InitializeAbilitySystemComponent(InAbilitySystemComponent);	
		
		PlayerCharacterOwner->OnEquipmentMeshChanged.AddWeakLambda(this, [this]
			(UBSEquipmentMeshComponent* EquipmentMeshComponent, USkeletalMesh* SkeletalMeshAsset)
		{
			
			if (EquipmentMeshComponent->MeshTag.MatchesTagExact(BSTags::EquipmentMesh_Head)) 
				HeadMeshAsset = SkeletalMeshAsset;
			if (EquipmentMeshComponent->MeshTag.MatchesTagExact(BSTags::EquipmentMesh_Chest)) 
				ChestMeshAsset = SkeletalMeshAsset;
			if (EquipmentMeshComponent->MeshTag.MatchesTagExact(BSTags::EquipmentMesh_Arms)) 
				ArmsMeshAsset = SkeletalMeshAsset;
			if (EquipmentMeshComponent->MeshTag.MatchesTagExact(BSTags::EquipmentMesh_Legs)) 
				LegsMeshAsset = SkeletalMeshAsset;
			
			BP_OnEquipmentMeshChanged(EquipmentMeshComponent->MeshTag, SkeletalMeshAsset);
		});
	}
}

void UBSPlayerAnimInstance::PreUpdateAnimation(const float DeltaSeconds)
{
	Super::PreUpdateAnimation(DeltaSeconds);
	if (!PlayerCharacterOwner || !bInitialized) return;
	UpdateRotation();
	UpdateFalling();
	
	float VelocityLength;
	FVector VelocityRef;
	
	if (CharacterOwner->IsLocallyControlled() && CharacterOwner->IsPlayerControlled())
	{
		VelocityRef = CharacterOwner->GetCharacterMovement()->GetCurrentAcceleration();
		float CurrentAccel = VelocityRef.Size();
		CurrentAccel = FMath::Clamp(CurrentAccel, 0.f, CharacterOwner->GetCharacterMovement()->GetMaxSpeed());
		VelocityLength = CurrentAccel;
	}
	else
	{
		VelocityRef = CharacterOwner->GetVelocity();
		VelocityRef.Z = 0;
		VelocityLength = VelocityRef.Size();
	}
	
	UpdateVelocity(VelocityRef.GetSafeNormal() * VelocityLength);
}

void UBSPlayerAnimInstance::OnAnyTagChanged(const FGameplayTag Tag, const int32 Count)
{
	Super::OnAnyTagChanged(Tag, Count);
	
	if (Tag.MatchesTag(BSTags::WeaponType) && Count > 0)
	{
		FGameplayTag OldTag = WeaponTypeTag;
		WeaponTypeTag = Tag;
		BP_OnWeaponTagChanged(WeaponTypeTag, OldTag);
	}
}

void UBSPlayerAnimInstance::BP_OnWeaponTagChanged_Implementation(FGameplayTag NewWeaponTypeTag,
                                                                 FGameplayTag OldWeaponTypeTag)
{
}

void UBSPlayerAnimInstance::BP_OnEquipmentMeshChanged_Implementation(FGameplayTag MeshTag,
                                                                     USkeletalMesh* NewSkeletalMesh)
{
	
}
