// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Actors/Mob/BSMobSubsystem.h"
#include "Actors/Mob/BSMobCharacter.h"
#include "Actors/Mob/BSMobSpawner.h"
#include "Components/Targeting/BSThreatComponent.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"

UBSMobSubsystem* UBSMobSubsystem::Get(const UObject* WorldContext)
{
	const UWorld* CurrentWorld = GEngine->GetWorldFromContextObjectChecked(WorldContext);
	return CurrentWorld->GetSubsystem<UBSMobSubsystem>();
}

void UBSMobSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UBSMobSubsystem::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (!GetWorld() || GetWorld()->bIsTearingDown) return;
	
	//Server only logic for mobs here
	if (GetWorld()->GetNetMode() != NM_DedicatedServer && GetWorld()->GetNetMode() != NM_ListenServer) return;
	
	CurrentSpawnTime += DeltaTime;
	
	if (CurrentSpawnTime >= SpawnerTickRate)
	{
		for (const auto MobSpawner : CurrentMobSpawners)
		{
			MobSpawner->TickMobSpawner();
		}
		CurrentSpawnTime = 0;
	}
}

bool UBSMobSubsystem::IsTickable() const
{
	return true;
}

bool UBSMobSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	if (!Super::ShouldCreateSubsystem(Outer))
	{
		return false;
	}

	const UWorld* World = Cast<UWorld>(Outer);
	return World && World->IsGameWorld();
}

TStatId UBSMobSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UBSMobSubsystem, STATGROUP_Tickables);
}

void UBSMobSubsystem::DeclareMob(ABSMobCharacter* MobCharacter, bool bAutoAssignTarget)
{
	if (!MobCharacters.Contains(MobCharacter))
	{
		if (bAutoAssignTarget)
		{
			MobCharacter->GetThreatComponent()->AddThreat(GetPlayerFromArrayDeterministic(), 1);
		}
		MobCharacters.Add(MobCharacter);
		OnMobSpawnDelegate.Broadcast(MobCharacter);
	}
}

void UBSMobSubsystem::UndeclareMob(ABSMobCharacter* MobCharacter)
{
	MobCharacters.Remove(MobCharacter);
}

void UBSMobSubsystem::RegisterSpawner(ABSMobSpawner* Spawner)
{
	CurrentMobSpawners.Add(Spawner);
}

void UBSMobSubsystem::UnregisterSpawner(ABSMobSpawner* Spawner)
{
	CurrentMobSpawners.Remove(Spawner);
}

TArray<ABSMobCharacter*> UBSMobSubsystem::GetMobCharacters()
{
	return MobCharacters;
}

AActor* UBSMobSubsystem::GetPlayerFromArrayDeterministic()
{
	TArray<AActor*> PlayerPawns;
	
	for (const auto PlayerState : GetWorld()->GetGameState()->PlayerArray)
	{
		if (PlayerState->GetPawn()) PlayerPawns.Add(PlayerState->GetPawn());
	}
	
	AActor* Target = PlayerPawns[FMath::Clamp(CurrentPlayerIt, 0, PlayerPawns.Num() - 1)];
	CurrentPlayerIt >= PlayerPawns.Num() - 1 ? CurrentPlayerIt = 0 : CurrentPlayerIt++;
	return Target;
}
