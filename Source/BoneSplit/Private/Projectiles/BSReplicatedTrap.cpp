// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Projectiles/BSReplicatedTrap.h"

#include "AbilitySystemComponent.h"
#include "CharacterAbilitySystem.h"
#include "NiagaraComponent.h"
#include "Components/TimelineComponent.h"
#include "Net/UnrealNetwork.h"

namespace BSGameplayTags
{
	extern UE_DEFINE_GAMEPLAY_TAG(Faction_Trap, "Faction.Trap");
}

// Sets default values
ABSReplicatedTrap::ABSReplicatedTrap(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	bAlwaysRelevant = true;
	SetNetUpdateFrequency(10);
	
	SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent")));
	
	Asc = CreateDefaultSubobject<UCharacterAbilitySystem>("AbilitySystemComponent");
	Asc->SetIsReplicated(true);
	Asc->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
	
	SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComponent"));
	SkeletalMeshComponent->SetupAttachment(GetRootComponent());
}

FGameplayTagContainer& ABSReplicatedTrap::GetFactionTags()
{
	return FactionTags;
}

bool ABSReplicatedTrap::HasAnyMatchingFactionTag(const FGameplayTagContainer InFactionTags)
{
	return FactionTags.HasAnyExact(InFactionTags);
}

void ABSReplicatedTrap::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
}

UAbilitySystemComponent* ABSReplicatedTrap::GetAbilitySystemComponent() const
{
	return Asc;
}

void ABSReplicatedTrap::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	ThrowTimeline.TickTimeline(DeltaSeconds);
}

void ABSReplicatedTrap::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABSReplicatedTrap, EndLocation);
	DOREPLIFETIME(ABSReplicatedTrap, ArcHeight);
	DOREPLIFETIME(ABSReplicatedTrap, FactionTags);
}

void ABSReplicatedTrap::BeginPlay()
{
	Super::BeginPlay();
	
	Asc->InitAbilityActorInfo(this, this);
	
	if (!ThrowCurve) return;

	// Bind the update callback
	FOnTimelineFloat UpdateCallback;
	UpdateCallback.BindUFunction(this, FName("OnThrowUpdate"));
	ThrowTimeline.AddInterpFloat(ThrowCurve, UpdateCallback);

	// Bind the finished callback
	FOnTimelineEvent FinishedCallback;
	FinishedCallback.BindUFunction(this, FName("OnThrowFinished"));
	ThrowTimeline.SetTimelineFinishedFunc(FinishedCallback);
	
	StartLocation = GetActorLocation();
	ThrowTimeline.PlayFromStart();
}

void ABSReplicatedTrap::OnThrowUpdate(float Alpha)
{
	FVector NewLocation = FMath::Lerp(StartLocation, EndLocation, Alpha);
	NewLocation.Z += ArcHeight * FMath::Sin(Alpha * PI);
	                                   
	SetActorLocation(NewLocation);
	
	BP_OnThrowTick(Alpha);
}

void ABSReplicatedTrap::BP_OnThrowTick_Implementation(float Alpha)
{
	
}

void ABSReplicatedTrap::OnThrowFinished()
{
	bHasLanded = true;
	OnLanded();
}

void ABSReplicatedTrap::OnLanded_Implementation()
{
}

