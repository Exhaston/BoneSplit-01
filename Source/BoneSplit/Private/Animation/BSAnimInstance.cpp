// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Animation/BSAnimInstance.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "KismetAnimationLibrary.h"
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
	
	FRotator TargetRotation;
	
	if (CharacterOwner->GetCharacterMovement()->bUseControllerDesiredRotation)
	{
		TargetRotation = CharacterOwner->GetBaseAimRotation() - CharacterOwner->GetActorRotation();
	}
	else
	{
		TargetRotation = FRotator::ZeroRotator;
	}
	
	TargetRotation.Pitch *= -1;
	AimRotation = 
		UKismetMathLibrary::RInterpTo(AimRotation, TargetRotation, DeltaSeconds, LookInterpSpeed);
	
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
	
	if (Tag.MatchesTag(BSTags::WeaponType_SwordNBoard))
	
	if (Count > 0)
	{
		OwnedGameplayTags.AddTag(ChangedTag);
		BP_OnGameplayTagAdded(ChangedTag, Count);
	}
	else
	{
		OwnedGameplayTags.RemoveTag(ChangedTag);
		BP_OnGameplayTagRemoved(ChangedTag, Count);
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
