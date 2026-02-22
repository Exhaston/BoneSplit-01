// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Animation/BSAnimInstance.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "KismetAnimationLibrary.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

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
		this, &UBSAnimInstance::NativeOnAnyTagChanged);
		
		OwnedGameplayTags.AppendTags(AbilitySystemComponent->GetOwnedGameplayTags());

		for (const auto OwnedTag : OwnedGameplayTags)
		{
			NativeOnAnyTagChanged(OwnedTag, AbilitySystemComponent->GetGameplayTagCount(OwnedTag));
		}
	
		bInitialized = true;
		OnAbilitySystemReady(InAbilitySystemComponent);
	}
}

void UBSAnimInstance::OnAbilitySystemReady_Implementation(UAbilitySystemComponent* OwnerAbilitySystemComponent)
{
	
}

void UBSAnimInstance::PreUpdateAnimation(const float DeltaSeconds)
{
	Super::PreUpdateAnimation(DeltaSeconds);
	if (!bInitialized || !CharacterOwner) return;
	UpdateRotation();
	UpdateFalling();
	UpdateVelocity(CharacterOwner->GetVelocity());
}

void UBSAnimInstance::NativeUpdateAnimation(const float DeltaSeconds)
{
	if (bInitialized && CharacterOwner)
	{
		Super::NativeUpdateAnimation(DeltaSeconds);
	}
}

void UBSAnimInstance::UpdateRotation()
{
	AimRotation = CharacterOwner->GetBaseAimRotation();
	
	AimRotation.Pitch = FRotator::NormalizeAxis(AimRotation.Pitch);
	AimRotation.Pitch *= -1;
}

void UBSAnimInstance::UpdateVelocity(const FVector Velocity)
{
	GravityVelocity = Velocity.Z;

	const float VelocityLength = FVector(Velocity.X, Velocity.Y, 0).Length();
	
	if (const float BaseWalkSpeed = CharacterOwner->GetCharacterMovement()->MaxWalkSpeed; 
		!FMath::IsNearlyZero(BaseWalkSpeed))
	{
		VelocityPercentage = (VelocityLength / BaseWalkSpeed) * 100.f;
	}
	
	const float TargetDirection = 
	UKismetAnimationLibrary::CalculateDirection(Velocity, CharacterOwner->GetActorRotation());
	
	VelocityDirection = FMath::RoundToFloat(TargetDirection / VelocityDirectionStepSize) * VelocityDirectionStepSize;
}

void UBSAnimInstance::UpdateFalling()
{
	IsFalling = CharacterOwner->GetCharacterMovement()->IsFalling();
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

void UBSAnimInstance::NativeOnAnyTagChanged(const FGameplayTag Tag, int32 Count)
{
	OnAnyTagChanged(Tag, Count);
	
	const FGameplayTag ChangedTag = Tag;
	
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

void UBSAnimInstance::OnAnyTagChanged(const FGameplayTag Tag, int32 Count)
{
	
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
