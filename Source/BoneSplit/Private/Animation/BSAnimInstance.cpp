// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Animation/BSAnimInstance.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "Chooser.h"
#include "KismetAnimationLibrary.h"
#include "Actors/Player/BSPlayerCharacter.h"
#include "Components/AbilitySystem/BSAttributeSet.h"
#include "Components/Inventory/BSEquipment.h"
#include "Components/Inventory/BSEquipmentMeshComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UBSAnimInstance::NativeInitializeAnimation()
{
	//Set before init to force anims into the correct start pos
	NormalizedAnimationOffset = FMath::RandRange(0.0, 1.0); 
	Super::NativeInitializeAnimation();
}

void UBSAnimInstance::InitializeAbilitySystemComponent(UAbilitySystemComponent* InAbilitySystemComponent)
{
	CharacterOwner = Cast<ACharacter>(TryGetPawnOwner());
	if (CharacterOwner)
	{
		AbilitySystemComponent = InAbilitySystemComponent;
		
		if (const ABSPlayerCharacter* PlayerCharacter = Cast<ABSPlayerCharacter>(CharacterOwner))
		{
			HeadMeshAsset = PlayerCharacter->HeadComponent->GetSkeletalMeshAsset();
			PlayerCharacter->HeadComponent->OnSkeletalMeshSetDelegate.AddWeakLambda(
				this, [this](USkeletalMesh* SkeletalMeshAsset)
			{
					HeadMeshAsset = SkeletalMeshAsset;
					BP_OnEquipmentMeshChanged(BSTags::EquipmentMesh_Head, HeadMeshAsset);
			});	
			ChestMeshAsset = PlayerCharacter->GetMesh()->GetSkeletalMeshAsset();
			Cast<UBSEquipmentMeshComponent>(PlayerCharacter->GetMesh())->OnSkeletalMeshSetDelegate.AddWeakLambda(
				this, [this](USkeletalMesh* SkeletalMeshAsset)
			{
					ChestMeshAsset = SkeletalMeshAsset;
					BP_OnEquipmentMeshChanged(BSTags::EquipmentMesh_Chest, ChestMeshAsset);
			});			
			LegsMeshAsset = PlayerCharacter->LegsComponent->GetSkeletalMeshAsset();
			PlayerCharacter->LegsComponent->OnSkeletalMeshSetDelegate.AddWeakLambda(
				this, [this](USkeletalMesh* SkeletalMeshAsset)
			{
					LegsMeshAsset = SkeletalMeshAsset;
					BP_OnEquipmentMeshChanged(BSTags::Equipment_Legs, LegsMeshAsset);
			}); 	
			ArmsMeshAsset = PlayerCharacter->ArmsComponent->GetSkeletalMeshAsset();
			PlayerCharacter->ArmsComponent->OnSkeletalMeshSetDelegate.AddWeakLambda(
				this, [this](USkeletalMesh* SkeletalMeshAsset)
			{
					ArmsMeshAsset = SkeletalMeshAsset;
					BP_OnEquipmentMeshChanged(BSTags::EquipmentMesh_Arms, ArmsMeshAsset);
			});
		}
		
		InAbilitySystemComponent->RegisterGenericGameplayTagEvent().AddUObject(
		this, &UBSAnimInstance::NativeOnTagEvent);
		
		OwnedGameplayTags.AppendTags(AbilitySystemComponent->GetOwnedGameplayTags());

		for (const auto OwnedTag : OwnedGameplayTags)
		{
			NativeOnTagEvent(OwnedTag, AbilitySystemComponent->GetGameplayTagCount(OwnedTag));
		}
	
		bInitialized = true;
		OnAbilitySystemReady(InAbilitySystemComponent);
	}
}

void UBSAnimInstance::OnAbilitySystemReady_Implementation(UAbilitySystemComponent* OwnerAbilitySystemComponent)
{
}

void UBSAnimInstance::NativeUpdateAnimation(const float DeltaSeconds)
{
	if (!bInitialized) return;
	
	AimRotation = CharacterOwner->GetBaseAimRotation();
	
	AimRotation.Pitch = FRotator::NormalizeAxis(AimRotation.Pitch);
	AimRotation.Pitch *= -1;
	
	float VelocityLength;
	const FVector Velocity = CharacterOwner->GetCharacterMovement()->Velocity;
	GravityVelocity = Velocity.Z;
	
	if (CharacterOwner->IsLocallyControlled() && CharacterOwner->IsPlayerControlled())
	{
		float CurrentAccel = CharacterOwner->GetCharacterMovement()->GetCurrentAcceleration().Size();
		CurrentAccel = FMath::Clamp(CurrentAccel, 0.f, CharacterOwner->GetCharacterMovement()->GetMaxSpeed());
		VelocityLength = CurrentAccel;
	}
	else
	{
		FVector VelocityXY = Velocity;
		VelocityXY.Z = 0;
		VelocityLength = VelocityXY.Size();
	}
	
	if (const float BaseWalkSpeed = CharacterOwner->GetCharacterMovement()->MaxWalkSpeed; 
		!FMath::IsNearlyZero(BaseWalkSpeed))
	{
		VelocityPercentage = (VelocityLength / BaseWalkSpeed) * 100.f;
	}
	
	IsFalling = CharacterOwner->GetCharacterMovement()->IsFalling();
	
	const float TargetDirection = 
		UKismetAnimationLibrary::CalculateDirection(CharacterOwner->GetVelocity(), CharacterOwner->GetActorRotation());
	
	VelocityDirection = FMath::RoundToFloat(TargetDirection / VelocityDirectionStepSize) * VelocityDirectionStepSize;
	
	Super::NativeUpdateAnimation(DeltaSeconds);
}

void UBSAnimInstance::BP_OnEquipmentMeshChanged_Implementation(FGameplayTag MeshTag, USkeletalMesh* NewSkeletalMesh)
{
	
}

UAbilitySystemComponent* UBSAnimInstance::GetAbilitySystemComponent()
{
	if (AbilitySystemComponent.IsValid()) return AbilitySystemComponent.Get();
	if (const IAbilitySystemInterface* AscInterface = Cast<IAbilitySystemInterface>(TryGetPawnOwner()))
	{
		AbilitySystemComponent = AscInterface->GetAbilitySystemComponent();
		return AbilitySystemComponent.Get();
	}
	return nullptr;
}

void UBSAnimInstance::NativeOnTagEvent(const FGameplayTag Tag, int32 Count)
{
	const FGameplayTag ChangedTag = Tag;
	
	if (Tag.MatchesTag(BSTags::WeaponType) && Count > 0)
	{
		WeaponTypeTag = ChangedTag;
	}
	
	if (Count != 0)
	{
		BP_OnGameplayTagAdded(ChangedTag, Count);
	}
	else
	{
		BP_OnGameplayTagRemoved(ChangedTag, Count);
	}
	
	if (AbilitySystemComponent.IsValid())
	{
		OwnedGameplayTags = AbilitySystemComponent.Get()->GetOwnedGameplayTags();
	}
	
	BP_OnGameplayTagChanged(ChangedTag, Count);
}

void UBSAnimInstance::BP_OnGameplayTagRemoved_Implementation(const FGameplayTag& Tag, const int32& Count)
{
}

void UBSAnimInstance::BP_OnGameplayTagAdded_Implementation(const FGameplayTag& Tag, const int32& Count)
{
}

void UBSAnimInstance::BP_OnGameplayTagChanged_Implementation(const FGameplayTag& Tag, const int32& Count)
{
	
}
