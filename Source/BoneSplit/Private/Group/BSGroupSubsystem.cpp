// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Group/BSGroupSubsystem.h"

#include "GameFramework/GameStateBase.h"

UBSGroupSubsystem* UBSGroupSubsystem::Get(const UWorld* World)
{
	if (!World) return nullptr;
	return World->GetSubsystem<UBSGroupSubsystem>();
}

void UBSGroupSubsystem::AddPlayerToGroup(ACharacter* InCharacter, UAbilitySystemComponent* PlayerAsc)
{
	JoinedPlayers.AddUnique(InCharacter);
	OnPlayerJoinedDelegate.Broadcast(InCharacter, PlayerAsc);
}

void UBSGroupSubsystem::SetPlayerReady(APawn* PlayerPawn, const bool bReady)
{
	bReady ? ReadyPlayers.AddUnique(PlayerPawn) : ReadyPlayers.Remove(PlayerPawn);
	
	if (IsAllPlayersReady())
	{
		OnPlayersReadyDelegate.Broadcast(ReadyPlayers);
	}
}

void UBSGroupSubsystem::FlushReadyPlayers()
{
	ReadyPlayers.Empty();
}

bool UBSGroupSubsystem::IsAllPlayersReady()
{
	if (GetWorld() && GetWorld()->GetGameState())
	{
		if (ReadyPlayers.Num() == GetWorld()->GetGameState()->PlayerArray.Num())
		{
			return true;
		}
	}
	
	return false;
}

FBSOnPlayersReady& UBSGroupSubsystem::GetOnPlayersReadyDelegate()
{
	return OnPlayersReadyDelegate;
}
