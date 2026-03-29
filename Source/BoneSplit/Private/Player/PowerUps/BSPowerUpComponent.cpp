// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Player/PowerUps/BSPowerUpComponent.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "CharacterAbilityBase.h"
#include "GameFramework/PlayerState.h"
#include "Player/PowerUps/BSPowerUpData.h"
#include "Player/PowerUps/BSPowerUpEffect.h"
#include "Player/PowerUps/BSPowerUpPickup.h"
#include "Player/PowerUps/BSPowerUpPool.h"


UBSPowerUpComponent::UBSPowerUpComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

UBSPowerUpComponent* UBSPowerUpComponent::GetPowerUpComponent(APlayerState* OwnerPS)
{
	return OwnerPS->GetComponentByClass<UBSPowerUpComponent>();
}

void UBSPowerUpComponent::SpawnPowerUpForPlayer(const FVector Location, const int32 MaxChoices)
{
	if (!PoolData) return;

	// --- 1. Build the eligible pool (filter out maxed-out power-ups) ---
	TArray<FBSPowerUpRow> EligibleRows;
	float TotalWeight = 0.f;

	for (const FBSPowerUpRow& Row : PoolData->PowerUps)
	{
		if (!Row.PowerDataAsset) continue;

		const int32* TimesGiven = NumGiven.Find(Row.PowerDataAsset);
		const int32 CurrentCount = TimesGiven ? *TimesGiven : 0;

		if (CurrentCount < Row.MaxCount)
		{
			EligibleRows.Add(Row);
			TotalWeight += Row.Weight;
		}
	}

	if (EligibleRows.IsEmpty() || TotalWeight <= 0.f) return;

	// --- 2. Pick NumChoices unique entries ---
	FBSAvailableChoice NewChoice;
	NewChoice.PowerUpGuid = FGuid::NewGuid();

	const int32 PickCount = FMath::Min(MaxChoices, EligibleRows.Num());

	for (int32 i = 0; i < PickCount; i++)
	{
		UBSPowerUpData* Picked = PickWeightedRandom(EligibleRows, TotalWeight);
		if (!Picked) break;

		NewChoice.ChoiceData.Add(Picked);

		// Remove from local eligible list so the next pick can't duplicate it
		const int32 RemovedIdx = EligibleRows.IndexOfByPredicate([Picked](const FBSPowerUpRow& Row)
		{
			return Row.PowerDataAsset == Picked;
		});

		if (RemovedIdx != INDEX_NONE)
		{
			TotalWeight -= EligibleRows[RemovedIdx].Weight;
			EligibleRows.RemoveAtSwap(RemovedIdx);
		}
	}

	if (NewChoice.ChoiceData.IsEmpty()) return;

	AvailableChoices.Add(NewChoice);
	Client_SpawnPowerUp(Location, NewChoice);
}

UBSPowerUpData* UBSPowerUpComponent::PickWeightedRandom(const TArray<FBSPowerUpRow>& EligibleRows, float TotalWeight)
{
	const float Roll = FMath::FRandRange(0.f, TotalWeight);
	float Accumulated = 0.f;

	for (const FBSPowerUpRow& Row : EligibleRows)
	{
		Accumulated += Row.Weight;
		if (Roll <= Accumulated)
		{
			return Row.PowerDataAsset;
		}
	}

	// Fallback for floating point edge cases
	return EligibleRows.Last().PowerDataAsset;
}

void UBSPowerUpComponent::ClaimPowerUp(const FBSAvailableChoice& InChoice, const int32 ChoiceIndex)
{
	if (!InChoice.ChoiceData.IsValidIndex(ChoiceIndex)) return; //Don't even bother the server if we screwed up
	Server_ClaimPowerUp(InChoice, ChoiceIndex);
}

void UBSPowerUpComponent::TransferPowerUps(UBSPowerUpComponent* NewComponent)
{
}

void UBSPowerUpComponent::Server_ClaimPowerUp_Implementation(const FBSAvailableChoice& Choice, int32 Index)
{
	const FBSAvailableChoice* ServerChoice = AvailableChoices.FindByKey(Choice);
	
    if (!ServerChoice)
    {
        UE_LOG(LogTemp, Warning, 
        	TEXT("Server_ClaimPowerUp: GUID not found in AvailableChoices - possible cheat or desync"));
    	
        return;
    }
	
    if (!ServerChoice->ChoiceData.IsValidIndex(Index))
    {
        UE_LOG(LogTemp, Warning, 
        	TEXT("Server_ClaimPowerUp: Invalid index %d for ChoiceData"), Index);
    	
        return;
    }

    UBSPowerUpData* ClaimedData = ServerChoice->ChoiceData[Index];
    if (!ClaimedData)
    {
        UE_LOG(LogTemp, Warning, 
        	TEXT("Server_ClaimPowerUp: ChoiceData at index %d is null"), Index);
    	
        return;
    }
	
    int32& GivenCount = NumGiven.FindOrAdd(ClaimedData);
    GivenCount++;

	const IAbilitySystemInterface* AscI = Cast<IAbilitySystemInterface>(GetOwner());
	if (!AscI) return;
	
	if (UAbilitySystemComponent* Asc = AscI->GetAbilitySystemComponent())
	{
		Asc->AddLooseGameplayTags(ClaimedData->GrantedTags);
		
		FGameplayEffectContextHandle EffectContext = Asc->MakeEffectContext();
		EffectContext.AddSourceObject(this);

		for (const TSubclassOf<UBSPowerUpEffect>& EffectClass : ClaimedData->GrantedEffects)
		{
			if (!EffectClass) continue;
			const FGameplayEffectSpecHandle SpecHandle = Asc->MakeOutgoingSpec(EffectClass, 1.f, EffectContext);
			if (SpecHandle.IsValid())
			{
				Asc->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
			}
		}
		
		for (const TSubclassOf<UCharacterAbilityBase>& AbilityClass : ClaimedData->GrantedAbilities)
		{
			if (!AbilityClass) continue;
			Asc->GiveAbility(FGameplayAbilitySpec(AbilityClass, 1, INDEX_NONE, this));
		}
	}
	
    AvailableChoices.RemoveAll([&Choice](const FBSAvailableChoice& Entry)
    {
        return Entry.PowerUpGuid == Choice.PowerUpGuid;
    });
}

void UBSPowerUpComponent::Client_SpawnPowerUp_Implementation(const FVector Location, const FBSAvailableChoice& Choice)
{
	UWorld* World = GetOwner()->GetWorld();
	
	FTransform SpawnTransform = FTransform::Identity;
	SpawnTransform.SetLocation(Location);
	ABSPowerUpPickup* NewPower = World->SpawnActorDeferred<ABSPowerUpPickup>(PowerUpClass, SpawnTransform, GetOwner());
	NewPower->ChoiceData = Choice;
	NewPower->FinishSpawning(SpawnTransform);
}

