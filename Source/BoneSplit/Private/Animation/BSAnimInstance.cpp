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
	NormalizedAnimationOffset = FMath::RandRange(0.0, 1.0);
	Super::NativeInitializeAnimation();
}

void UBSAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
	
	CharacterOwner = Cast<ACharacter>(TryGetPawnOwner());
	if (CharacterOwner && Cast<IAbilitySystemInterface>(CharacterOwner))
	{
		GetAbilitySystemComponent()->RegisterGenericGameplayTagEvent().AddUObject(
		this, &UBSAnimInstance::NativeOnGameplayTagAdded);
	
		bInitialized = true;
	}
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
	
	float CurrentVelocity;
	
	if (CharacterOwner->IsLocallyControlled())
	{
		  CurrentVelocity = CharacterOwner->GetCharacterMovement()->GetCurrentAcceleration().Size();
	}
	else
	{
		CurrentVelocity = CharacterOwner->GetCharacterMovement()->Velocity.Size();
	}
	
	if (const float MaxSpeed = CharacterOwner->GetCharacterMovement()->GetMaxSpeed(); 
		MaxSpeed > 0.f)
	{
		VelocityPercentage = (CurrentVelocity / MaxSpeed) * 100.0f;
	}
	
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

void UBSAnimInstance::NativeOnGameplayTagAdded(const FGameplayTag Tag, int32 Count)
{
	FGameplayTag ChangedTag = Tag;
	
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
