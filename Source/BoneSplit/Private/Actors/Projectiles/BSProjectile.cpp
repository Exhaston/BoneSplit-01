// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Actors/Projectiles/BSProjectile.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "AbilitiesExtensionLib.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"


// Sets default values
ABSProjectile::ABSProjectile(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	PrimaryActorTick.bCanEverTick = true;
	OverlapComponent = CreateDefaultSubobject<USphereComponent>("OverlapComponent");
	SetRootComponent(OverlapComponent);
	
	ProjectileMovementComponent = 
		CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->SetUpdatedComponent(OverlapComponent);
}

void ABSProjectile::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	
	if (!OtherActor || !GetOwner() || !GetAbilitySystemComponent()) return;
	if (HitActors.Contains(OtherActor)) return;
	if (const IAbilitySystemInterface* AscInterface = Cast<IAbilitySystemInterface>(OtherActor); 
		AscInterface && !OtherActor->IsA(StaticClass()))
	{
		if (AscInterface->GetAbilitySystemComponent() && OnAnyTargetHit(AscInterface->GetAbilitySystemComponent()))
		{
			HitActors.Add(OtherActor);
			CurrentHits++;
				
			if (CurrentHits >= MaxHits)
			{
				Destroy();
			}
		}
	}
}

bool ABSProjectile::OnAnyTargetHit_Implementation(UAbilitySystemComponent* TargetAsc)
{
	return false;
}

void ABSProjectile::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	SetCollisionProfiles();
}

void ABSProjectile::SetCollisionProfiles()
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

TArray<ABSProjectile*> ABSProjectile::SpawnProjectiles(
	AActor* InOwnerActor, 
	const TSubclassOf<ABSProjectile> 
	ProjectileClass, 
	const FTransform& InSpawnTransform,
	const int32 NumProjectiles,
	const float SpreadDegrees)
{
	TArray<ABSProjectile*> Projectiles;
	
	UWorld* World = InOwnerActor->GetWorld();
	if (NumProjectiles <= 0 || !InOwnerActor || !World) return Projectiles;

	APawn* NewInstigator = Cast<APawn>(InOwnerActor);
	
	FTransform NewProjectileTransform = InSpawnTransform;
	const FRotator BaseRotation = NewProjectileTransform.GetRotation().Rotator();
	const float MaxSpreadAngle = SpreadDegrees;
	const int32 TotalProjectiles = NumProjectiles;

	const float AngleStep = TotalProjectiles > 1 ? MaxSpreadAngle * 2.0f / (TotalProjectiles - 1) : 0.0f;

	for (int32 i = 0; i < TotalProjectiles; i++)
	{
		const float YawOffset = TotalProjectiles > 1 ? -MaxSpreadAngle + AngleStep * i : 0.0f;
    	
		//if (FMath::IsNearlyZero(YawOffset)) continue;
    	
		FRotator SpawnRotation = BaseRotation;
		SpawnRotation.Yaw += YawOffset;
		NewProjectileTransform.SetRotation(SpawnRotation.Quaternion());
    	
		ABSProjectile* ProjectileInstance = World->SpawnActorDeferred<ABSProjectile>(
		ProjectileClass, 
		NewProjectileTransform, 
		InOwnerActor, 
		NewInstigator, 
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	
		ProjectileInstance->FinishSpawning(NewProjectileTransform);
	}
	return Projectiles;
}

UAbilitySystemComponent* ABSProjectile::GetAbilitySystemComponent() const
{
	if (const IAbilitySystemInterface* AbilitySystemInterface = Cast<IAbilitySystemInterface>(GetOwner()))
	{
		return AbilitySystemInterface->GetAbilitySystemComponent();
	}
	return nullptr;
}

void ABSProjectile::NotifyHit(
	UPrimitiveComponent* MyComp, 
	AActor* Other,
	UPrimitiveComponent* OtherComp, 
	const bool bSelfMoved, 
	const FVector HitLocation, 
	const FVector HitNormal,
	const FVector NormalImpulse, 
	const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);
	if (bCanPassThroughStatic) return;
	Destroy();
}

