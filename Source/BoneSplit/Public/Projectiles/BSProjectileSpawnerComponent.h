// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BSProjectileSpawnerComponent.generated.h"


class ABSProjectile;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class BONESPLIT_API UBSProjectileSpawnerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	
	UBSProjectileSpawnerComponent();
	
	UFUNCTION(BlueprintCallable)
	static UBSProjectileSpawnerComponent* GetProjectileComponentFromActor(AActor* Target);
	
	UFUNCTION(BlueprintCallable)
	void SpawnProjectiles(
		TSubclassOf<ABSProjectile> ProjectileClass, 
		const FTransform& InSpawnTransform, 
		int32 NumProjectiles = 0, 
		float SpreadDegrees = 15);
	
	UFUNCTION(BlueprintCallable)
	void SpawnGenericActor(
		TSubclassOf<AActor> ActorToSpawn, 
		const FTransform& InSpawnTransform);
	
private:
	
	UFUNCTION(NetMulticast, Reliable)
	void NetMulticast_SpawnGenericActor(
		TSubclassOf<AActor> ActorToSpawn,
		const FTransform& InSpawnTransform);
	
	UFUNCTION(Server, Reliable)
	void Server_SpawnGenericActor(
		TSubclassOf<AActor> ActorToSpawn,
		const FTransform& InSpawnTransform);
	
	bool IsOwnerAuthoritative();
	bool IsOwnerLocallyControlled();
	
	UFUNCTION(NetMulticast, Reliable)
	void NetMulticast_SpawnProjectiles(
		TSubclassOf<ABSProjectile> ProjectileClass, 
		const FTransform& InSpawnTransform, 
		int32 NumProjectiles = 0, 
		float SpreadDegrees = 15);
	
	UFUNCTION(NetMulticast, Reliable)
	void Server_SpawnProjectiles(
		TSubclassOf<ABSProjectile> ProjectileClass, 
		const FTransform& InSpawnTransform, 
		int32 NumProjectiles = 0, 
		float SpreadDegrees = 15);
};
