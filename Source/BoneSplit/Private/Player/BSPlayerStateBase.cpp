// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Player/BSPlayerStateBase.h"

#include "Abilities/BSAbilitySystem.h"
#include "Abilities/BSExtendedAttributeSet.h"
#include "Player/BSPlayerCharacterBase.h"
#include "Player/PowerUps/BSPowerUpComponent.h"

ABSPlayerStateBase::ABSPlayerStateBase(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bReplicates = true;
	bAlwaysRelevant = true;
	SetNetUpdateFrequency(64);
	
	AbilitySystemComponent = CreateDefaultSubobject<UBSAbilitySystem>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Full);
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetOwnerActor(this);
	
	AttributeSet = CreateDefaultSubobject<UBSExtendedAttributeSet>(TEXT("AttributeSet"));
	
	PowerUpComponent = CreateDefaultSubobject<UBSPowerUpComponent>(TEXT("PowerUpComponent"));
}

void ABSPlayerStateBase::ClientInitialize(AController* C)
{
	Super::ClientInitialize(C);
	
	if (ABSPlayerCharacterBase* PlayerCharacterBase = GetPawn<ABSPlayerCharacterBase>())
	{
		PlayerCharacterBase->InitAbilitySystem();
	}
}

void ABSPlayerStateBase::CopyProperties(APlayerState* PlayerState)
{
	Super::CopyProperties(PlayerState);
	
	ABSPlayerStateBase* NewPS = Cast<ABSPlayerStateBase>(PlayerState);
	if (!NewPS || !NewPS->HasAuthority()) return;
	
	UAbilitySystemComponent* NewAsc = NewPS->GetAbilitySystemComponent();
	const UAbilitySystemComponent* OldAsc = GetAbilitySystemComponent();
	
	TArray<FGameplayAttribute> Attributes;
	GetAbilitySystemComponent()->GetAllAttributes(Attributes);
	for (auto& Attribute : Attributes)
	{
		NewAsc->SetNumericAttributeBase(Attribute, OldAsc->GetNumericAttributeBase(Attribute));
	}
	
	TArray<FGameplayTagContainer> SpecTags;
	
	TArray<FGameplayEffectSpec> SpecCopies;

	TArray<FActiveGameplayEffectHandle> ActiveGEHandles = OldAsc->GetActiveEffects(
		FGameplayEffectQuery::MakeQuery_MatchNoEffectTags({}));

	for (const auto& ActiveGEHandle : ActiveGEHandles)
	{
		if (!ActiveGEHandle.IsValid()) continue;
		
		const FActiveGameplayEffect* ActiveGE = OldAsc->GetActiveGameplayEffect(ActiveGEHandle);
		if (!ActiveGE) continue;
		
		const FGameplayEffectContextHandle Context = NewAsc->MakeEffectContext();

		FGameplayEffectSpecHandle SpecHandle = NewPS->GetAbilitySystemComponent()->MakeOutgoingSpec(
			ActiveGE->Spec.Def->GetClass(),
			1,
			Context
		);
		
		FGameplayTagContainer SpecCopyTags;
		ActiveGE->Spec.GetAllGrantedTags(SpecCopyTags);
		SpecTags.Add(SpecCopyTags);
		SpecHandle.Data->SetStackCount(ActiveGE->Spec.GetStackCount());
		SpecHandle.Data->SetDuration(ActiveGE->GetTimeRemaining(OldAsc->GetWorld()->GetTimeSeconds()), 
			true);
		NewPS->GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);
	}
	
	TArray<FGameplayTag> ActualTags;

	for (auto& Tag : GetAbilitySystemComponent()->GetOwnedGameplayTags().GetGameplayTagArray())
	{
		for (int i = 0; i < GetAbilitySystemComponent()->GetTagCount(Tag); ++i)
		{
			ActualTags.Add(Tag);
		}
	}

	for (auto& SpecTagContainer : SpecTags)
	{
		for (auto SpecTag : SpecTagContainer.GetGameplayTagArray())
		{
			ActualTags.RemoveSingle(SpecTag);
		}
	}

	for (auto& ActualTag : ActualTags)
	{
		NewPS->GetAbilitySystemComponent()->AddLooseGameplayTag(
			ActualTag, 1, EGameplayTagReplicationState::TagAndCountToAll);
	}
	
	NewPS->SetHasAbilitySystemData();
}

UAbilitySystemComponent* ABSPlayerStateBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ABSPlayerStateBase::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
	return AbilitySystemComponent->GetOwnedGameplayTags(TagContainer);
}

bool ABSPlayerStateBase::HasMatchingGameplayTag(const FGameplayTag TagToCheck) const
{
	return AbilitySystemComponent->HasMatchingGameplayTag(TagToCheck);
}

bool ABSPlayerStateBase::HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	return AbilitySystemComponent->HasAllMatchingGameplayTags(TagContainer);
}

bool ABSPlayerStateBase::HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	return AbilitySystemComponent->HasAnyMatchingGameplayTags(TagContainer);
}

UBSExtendedAttributeSet* ABSPlayerStateBase::GetAttributeSet()
{
	return AttributeSet;
}
