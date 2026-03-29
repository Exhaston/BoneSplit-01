// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Projectiles/BSProjectileSpawnerComponent.h"

#include "Projectiles/BSProjectile.h"


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

void UBSProjectileSpawnerComponent::SpawnGenericActor(TSubclassOf<AActor> ActorToSpawn,
	const FTransform& InSpawnTransform)
{
	Server_SpawnGenericActor(ActorToSpawn, InSpawnTransform);
}

void UBSProjectileSpawnerComponent::NetMulticast_SpawnGenericActor_Implementation(TSubclassOf<AActor> ActorToSpawn,
                                                                                  const FTransform& InSpawnTransform)
{
	if (GetOwner() && GetOwner()->GetWorld())
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = GetOwner();
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		GetOwner()->GetWorld()->SpawnActor<AActor>(ActorToSpawn, InSpawnTransform, SpawnParams);
	}
}

void UBSProjectileSpawnerComponent::Server_SpawnGenericActor_Implementation(TSubclassOf<AActor> ActorToSpawn,
	const FTransform& InSpawnTransform)
{
	NetMulticast_SpawnGenericActor(ActorToSpawn, InSpawnTransform);
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

