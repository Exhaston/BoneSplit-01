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
		
		PlayerCharacterOwner->OnSkeletalMeshSet.AddWeakLambda(this, [this]
			(FGameplayTag MeshTag, USkeletalMesh* SkelMesh)
		{
			if (MeshTag.MatchesTagExact(BSTags::Equipment_Head)) HeadMeshAsset = SkelMesh;
			if (MeshTag.MatchesTagExact(BSTags::Equipment_Chest)) ChestMeshAsset = SkelMesh;
			if (MeshTag.MatchesTagExact(BSTags::Equipment_Legs)) LegsMeshAsset = SkelMesh;
			if (MeshTag.MatchesTagExact(BSTags::Equipment_Arms)) ArmsMeshAsset = SkelMesh;
			
			BP_OnEquipmentChanged();
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

void UBSPlayerAnimInstance::BP_OnEquipmentChanged_Implementation()
{
}

void UBSPlayerAnimInstance::BP_OnWeaponTagChanged_Implementation(FGameplayTag NewWeaponTypeTag,
                                                                 FGameplayTag OldWeaponTypeTag)
{
}
