// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "PawnInitializationComponent.h"

#include "AbilitySystemInterface.h"
#include "CharacterAbilitySystem.h"
#include "GameFramework/PlayerState.h"

// Sets default values for this component's properties
UPawnInitializationComponent::UPawnInitializationComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);

	AbilitySystemComponent = nullptr;
}

bool UPawnInitializationComponent::TryInitAbilitySystemForPlayer(UCharacterAbilitySystem* CharacterAbilitySystem, AActor* TargetOwnerActor)
{
	if (CharacterAbilitySystem == AbilitySystemComponent) return false; //Already init

	if (!TargetOwnerActor) return false;
	APawn* CurrentPawn = GetPawnChecked<APawn>();
	
	if ((CurrentPawn->HasAuthority() || CurrentPawn->IsLocallyControlled()) && !CurrentPawn->GetController())
		return false; //Local player or server had no controller yet.
	
	if (!CurrentPawn->IsPlayerControlled()) return false; //Require Player State

	if (const AActor* ExistingAvatar = CharacterAbilitySystem->GetAvatarActor(); ExistingAvatar && ExistingAvatar != CurrentPawn)
	{
		if (UPawnInitializationComponent* OtherExtensionComponent = FindPawnExtensionComponent(ExistingAvatar))
		{
			OtherExtensionComponent->UninitializeAbilitySystem();
		}
	}
	
	AbilitySystemComponent = CharacterAbilitySystem;
	CharacterAbilitySystem->InitAbilityActorInfo(TargetOwnerActor, CurrentPawn);
	
	OnAbilitySystemInitialized.Broadcast();
	return true;
}

bool UPawnInitializationComponent::TryInitAbilitySystemForAI(UCharacterAbilitySystem* CharacterAbilitySystem)
{
	if (CharacterAbilitySystem == AbilitySystemComponent) return false; //Already init

	APawn* CurrentPawn = GetPawnChecked<APawn>();

	if (const AActor* ExistingAvatar = CharacterAbilitySystem->GetAvatarActor(); ExistingAvatar && ExistingAvatar != CurrentPawn)
	{
		if (UPawnInitializationComponent* OtherExtensionComponent = FindPawnExtensionComponent(ExistingAvatar))
		{
			OtherExtensionComponent->UninitializeAbilitySystem();
		}
	}
	
	AbilitySystemComponent = CharacterAbilitySystem;
	CharacterAbilitySystem->InitAbilityActorInfo(CurrentPawn, CurrentPawn);
	
	OnAbilitySystemInitialized.Broadcast();
	return true;
}

void UPawnInitializationComponent::UninitializeAbilitySystem()
{
	if (!AbilitySystemComponent)
	{
		return;
	}
	
	if (AbilitySystemComponent->GetAvatarActor() == GetOwner())
	{

		AbilitySystemComponent->CancelAbilities(nullptr);
		AbilitySystemComponent->RemoveAllGameplayCues();

		if (AbilitySystemComponent->GetOwnerActor() != nullptr)
		{
			AbilitySystemComponent->SetAvatarActor(nullptr);
		}
		else
		{
			AbilitySystemComponent->ClearActorInfo();
		}

		OnAbilitySystemUninitialized.Broadcast();
	}

	AbilitySystemComponent = nullptr;
}

void UPawnInitializationComponent::HandleControllerChanged()
{
	if (AbilitySystemComponent && AbilitySystemComponent->GetAvatarActor() == GetPawnChecked<APawn>())
	{
		ensure(AbilitySystemComponent->AbilityActorInfo->OwnerActor == AbilitySystemComponent->GetOwnerActor());
		if (AbilitySystemComponent->GetOwnerActor() == nullptr)
		{
			UninitializeAbilitySystem();
		}
		else
		{
			AbilitySystemComponent->RefreshAbilityActorInfo();
		}
	}
}

void UPawnInitializationComponent::OnAbilitySystemInitialized_RegisterAndCall(
	FSimpleMulticastDelegate::FDelegate Delegate)
{
	if (!OnAbilitySystemInitialized.IsBoundToObject(Delegate.GetUObject()))
	{
		OnAbilitySystemInitialized.Add(Delegate);
	}

	if (AbilitySystemComponent)
	{
		Delegate.Execute();
	}
}

void UPawnInitializationComponent::OnAbilitySystemUninitialized_Register(FSimpleMulticastDelegate::FDelegate Delegate)
{
	if (!OnAbilitySystemUninitialized.IsBoundToObject(Delegate.GetUObject()))
	{
		OnAbilitySystemUninitialized.Add(Delegate);
	}
}

