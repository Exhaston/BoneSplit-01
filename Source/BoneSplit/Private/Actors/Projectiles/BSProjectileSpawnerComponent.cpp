// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Actors/Projectiles/BSProjectileSpawnerComponent.h"

#include "Actors/Projectiles/BSProjectile.h"


UBSProjectileSpawnerComponent::UBSProjectileSpawnerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	
	SetIsReplicatedByDefault(true);
}

UBSProjectileSpawnerComponent* UBSProjectileSpawnerComponent::GetProjectileComponentFromActor(AActor* Target)
{
	return Target->GetComponentByClass<UBSProjectileSpawnerComponent>();
}

void UBSProjectileSpawnerComponent::SpawnProjectiles(TSubclassOf<ABSProjectile> ProjectileClass,
                                                     const FTransform& InSpawnTransform, int32 NumProjectiles, float SpreadDegrees)
{
	if (NumProjectiles <= 0) return;
	//Implement prediction key validation here.
	Server_SpawnProjectiles(ProjectileClass, InSpawnTransform, NumProjectiles, SpreadDegrees);
}

bool UBSProjectileSpawnerComponent::IsOwnerAuthoritative()
{
	return GetOwner()->HasAuthority();
}

bool UBSProjectileSpawnerComponent::IsOwnerLocallyControlled()
{
	return GetOwner()->HasLocalNetOwner();
}

void UBSProjectileSpawnerComponent::Server_SpawnProjectiles_Implementation(TSubclassOf<ABSProjectile> ProjectileClass,
                                                                           const FTransform& InSpawnTransform, int32 NumProjectiles, float SpreadDegrees)
{
	NetMulticast_SpawnProjectiles(ProjectileClass, InSpawnTransform, NumProjectiles, SpreadDegrees);
}

void UBSProjectileSpawnerComponent::NetMulticast_SpawnProjectiles_Implementation(
	const TSubclassOf<ABSProjectile> ProjectileClass, const FTransform& InSpawnTransform, int32 NumProjectiles,
	const float SpreadDegrees)
{
	ABSProjectile::SpawnProjectiles(GetOwner(), ProjectileClass, InSpawnTransform, NumProjectiles, SpreadDegrees);
}

