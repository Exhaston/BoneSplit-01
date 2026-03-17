// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "AbilityBufferComponent.h"

#include "CharacterAbilitySystem.h"
#include "EnhancedInputComponent.h"


UAbilityBufferComponent::UAbilityBufferComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 0;
}

void UAbilityBufferComponent::BindAbilityInputActions(UEnhancedInputComponent* EnhancedInputComponent)
{
	for (auto AbilityIDPair : AbilityInputActions)
	{
		EnhancedInputComponent->BindActionValueLambda(AbilityIDPair.InputAction, 
			AbilityIDPair.bRetriggerEveryFrame ? ETriggerEvent::Triggered : ETriggerEvent::Started,
			[this, AbilityIDPair](const FInputActionValue& Value)
			{
				if (!AbilityIDPair.bRetriggerEveryFrame) TryBufferAbilityByID(AbilityIDPair.AbilityInputID);
				NotifyInputPressed(AbilityIDPair.AbilityInputID);
			});
		EnhancedInputComponent->BindActionValueLambda(AbilityIDPair.InputAction, ETriggerEvent::Completed,
		[this, AbilityIDPair](const FInputActionValue& Value)
		{
			   NotifyInputReleased(AbilityIDPair.AbilityInputID);
		});
	}
}

UAbilityBufferComponent* UAbilityBufferComponent::GetBufferComponentFromController(const AController* PC)
{
	return PC ? PC->GetComponentByClass<UAbilityBufferComponent>() : nullptr;
}

void UAbilityBufferComponent::SetAbilitySystemComponent(UCharacterAbilitySystem* InAsc)
{
	check(InAsc);
	CharacterAbilitySystem = InAsc;
}

bool UAbilityBufferComponent::GetIsBufferedOrPressed(const int32 InputID) const
{
	return PressedInputs.Contains(InputID) || GetIsAbilityBuffered(InputID);
}

bool UAbilityBufferComponent::GetIsAbilityBuffered(int32 InputID) const
{
	return BufferedAbilities.ContainsByPredicate([this, InputID](const FAbilityBufferData& Ability)
	{
		return Ability.AbilityInputID == InputID;
	});
}

bool UAbilityBufferComponent::TryActivateAbility(const int32 InputID) const
{
	const FGameplayAbilitySpec* FoundSpec = CharacterAbilitySystem->FindAbilitySpecFromInputID(InputID);
	return FoundSpec ? CharacterAbilitySystem->TryActivateAbility(FoundSpec->Handle) : false;
}

void UAbilityBufferComponent::NotifyInputPressed(const int32 InputID)
{
	if (CharacterAbilitySystem) CharacterAbilitySystem->PressInputID(InputID);
	PressedInputs.AddUnique(InputID);
}

void UAbilityBufferComponent::NotifyInputReleased(const int32 InputID)
{
	if (CharacterAbilitySystem) CharacterAbilitySystem->ReleaseInputID(InputID);
	PressedInputs.Remove(InputID);
}

void UAbilityBufferComponent::BufferAbility(TEnumAsByte<EAbilityBufferMode> BufferMode, int32 AbilityID)
{
	if (BufferMode == EABT_None) return;
	
	BufferedAbilities.RemoveAll([this, AbilityID, BufferMode] (const FAbilityBufferData& Ability)
	{
		return Ability.AbilityInputID == AbilityID || (BufferMode == EABT_Unique && Ability.BufferChannel == EABT_Unique);
	}); //Remove if we are refreshing same ability buffer or remove past unique buffer if new ability is unique
	
	FAbilityBufferData NewBufferedAbility;
	NewBufferedAbility.BufferChannel = BufferMode;
	NewBufferedAbility.AbilityInputID = AbilityID;
	NewBufferedAbility.TimeRemaining = BufferTime;
	BufferedAbilities.Add(NewBufferedAbility);
}

void UAbilityBufferComponent::TryBufferAbilityByID(const int32 InputID)
{
	if (!CharacterAbilitySystem) return;
	
	const FGameplayAbilitySpec* FoundSpec = CharacterAbilitySystem->FindAbilitySpecFromInputID(InputID);
	
	if (!FoundSpec) return;
	
	const UCharacterAbilityBase* CharacterPrimaryInstance = 
		Cast<UCharacterAbilityBase>(FoundSpec->GetPrimaryInstance());
	
	if (!CharacterPrimaryInstance) return;
	
	if (!CharacterAbilitySystem->TryActivateAbility(FoundSpec->Handle) && CharacterPrimaryInstance->BufferChannel != EABT_None)
	{
		BufferAbility(CharacterPrimaryInstance->BufferChannel, InputID);
	}
}

void UAbilityBufferComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                            FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	//Only need the local owner to buffer their abilities, 
	//server doesn't handle input here
	if (!GetOwner()->HasLocalNetOwner() || !CharacterAbilitySystem) return; 
	
	TArray<FAbilityBufferData> ToRemove;
	
	for (auto& BufferedAbility : BufferedAbilities)
	{
		const FGameplayAbilitySpec* FoundSpec = 
			CharacterAbilitySystem->FindAbilitySpecFromInputID(BufferedAbility.AbilityInputID);
		
		if (!FoundSpec || (FoundSpec && CharacterAbilitySystem->TryActivateAbility(FoundSpec->Handle)) 
			|| BufferedAbility.TimeRemaining <= 0)
		{
			ToRemove.Add(BufferedAbility);
		}
		else
		{
			BufferedAbility.TimeRemaining -= DeltaTime;
		}
	}
	
	BufferedAbilities.RemoveAll([&ToRemove](const FAbilityBufferData& Ability)
	{
		return ToRemove.Contains(Ability);
	});
}

