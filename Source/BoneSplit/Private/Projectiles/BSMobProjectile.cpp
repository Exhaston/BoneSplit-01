// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Projectiles/BSMobProjectile.h"

#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

ABSMobProjectile::ABSMobProjectile(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	
	OverlapComponent = CreateDefaultSubobject<USphereComponent>("OverlapComponent");
	SetRootComponent(OverlapComponent);
	
	ProjectileMovementComponent = 
		CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	
	ProjectileMovementComponent->SetUpdatedComponent(OverlapComponent);
}
									 
void ABSMobProjectile::SetCollisionProfiles()
{
	OverlapComponent->SetGenerateOverlapEvents(true);
	OverlapComponent->SetCollisionProfileName(TEXT("Custom"));
	OverlapComponent->SetCollisionEnabled(ECollisionEnabled::Type::QueryOnly);
	OverlapComponent->SetCollisionObjectType(ECC_Pawn);
	OverlapComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	OverlapComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	OverlapComponent->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	OverlapComponent->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	OverlapComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void ABSMobProjectile::BeginPlay()
{
	Super::BeginPlay();
	ProjectileMovementComponent->HomingTargetComponent = TargetActor->GetRootComponent();
	ProjectileMovementComponent->bIsHomingProjectile = IsValid(TargetActor);
	SetCollisionProfiles();
}
