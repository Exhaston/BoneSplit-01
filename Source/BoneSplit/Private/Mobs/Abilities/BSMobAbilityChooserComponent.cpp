// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Mobs/Abilities/BSMobAbilityChooserComponent.h"

#include "AbilitySystemComponent.h"
#include "Mobs/Abilities/BSMobAbility.h"


UBSMobAbilityChooserComponent::UBSMobAbilityChooserComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.TickInterval = 1;
}

void UBSMobAbilityChooserComponent::BeginPlay()
{
	Super::BeginPlay();
}


void UBSMobAbilityChooserComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                                  FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	

}

FGameplayAbilitySpecHandle UBSMobAbilityChooserComponent::EvaluateAndFindAbility()
{
	if (!AbilitySystemComponent) return FGameplayAbilitySpecHandle();

    // Build candidate list — only abilities that pass GetValidActor + CanActivate
    struct FCandidate
    {
        FGameplayAbilitySpecHandle Handle;
        float Weight;
    };
    TArray<FCandidate> Candidates;
    float TotalWeight = 0.f;

    for (const FGameplayAbilitySpecHandle& Handle : GrantedHandles)
    {
    	if (!Handle.IsValid()) continue;
        FGameplayAbilitySpec* Spec = AbilitySystemComponent->FindAbilitySpecFromHandle(Handle);
        if (!Spec) continue;

        UBSMobAbility* AbilityCDO = Cast<UBSMobAbility>(Spec->GetPrimaryInstance());
        if (!AbilityCDO) continue;

        // Your validity + can-activate checks
        if (!AbilityCDO->HasValidTarget()) continue;
        if (!AbilityCDO->CanActivateAbility(Handle, AbilitySystemComponent->AbilityActorInfo.Get(),
            nullptr, nullptr, nullptr)) continue;

        const float* WeightPtr = AbilityWeightMap.Find(Spec->Ability->GetClass());
        const float Weight = WeightPtr ? *WeightPtr : 1.f;

        Candidates.Add({ Handle, Weight });
        TotalWeight += Weight;
    }
	
	if (Candidates.IsEmpty() || TotalWeight <= 0.f) return FGameplayAbilitySpecHandle();
	
    // Weighted random roll
    float Roll = FMath::FRandRange(0.f, TotalWeight);
    for (const FCandidate& Candidate : Candidates)
    {
        Roll -= Candidate.Weight;
        if (Roll <= 0.f)
        {
            return Candidate.Handle;
        }
    }

    // Floating point safety — return last candidate
    return Candidates.Last().Handle;
}

void UBSMobAbilityChooserComponent::StartAbilityChooser(UAbilitySystemComponent* InAbilitySystem)
{
	if (!InAbilitySystem || !InAbilitySystem->IsOwnerActorAuthoritative()) return;
	AbilitySystemComponent = InAbilitySystem;
	for (auto Weights : AbilityWeightMap)
	{
		if (bAutoGrantAbilities)
		{
			FGameplayAbilitySpec NewSpec = FGameplayAbilitySpec(Weights.Key);
			NewSpec.Level = 1;
			GrantedHandles.Add(AbilitySystemComponent->GiveAbility(NewSpec));
		}
	}
	
	InAbilitySystem->GetWorld()->GetTimerManager().SetTimer(
		TickTimer, this, &UBSMobAbilityChooserComponent::OnComponentTickedFromTimer, TickRate, true, FMath::RandRange(0.f, TickRate));
}

void UBSMobAbilityChooserComponent::StopAbilityChooser()
{
	if (AbilitySystemComponent) AbilitySystemComponent->GetWorld()->GetTimerManager().ClearTimer(TickTimer);
}

void UBSMobAbilityChooserComponent::DestroyComponent(bool bPromoteChildren)
{
	StopAbilityChooser();
	Super::DestroyComponent(bPromoteChildren);
}

void UBSMobAbilityChooserComponent::OnComponentTickedFromTimer()
{
	if (AbilitySystemComponent && AbilitySystemComponent->IsOwnerActorAuthoritative())
	{
		if (const FGameplayAbilitySpecHandle ChosenHandle = EvaluateAndFindAbility(); ChosenHandle.IsValid())
		{
			AbilitySystemComponent->TryActivateAbility(ChosenHandle);
		}
	}
}

