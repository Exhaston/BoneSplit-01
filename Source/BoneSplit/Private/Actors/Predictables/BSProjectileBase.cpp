// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Actors/Predictables/BSProjectileBase.h"

#include "AbilitySystemComponent.h"
#include "Components/SphereComponent.h"
#include "Components/AbilitySystem/BSAbilityLibrary.h"
#include "Components/AbilitySystem/EffectBases/BSGameplayEffect.h"
#include "GameFramework/ProjectileMovementComponent.h"


FBSProjectileAlignment::FBSProjectileAlignment(
	const FTransform& SpawnTransform,
	const FVector& StartVelocity, 
	const FTransform& CameraTransform, 
	const float InAlignmentTime)
{
	RealStartPos = SpawnTransform.GetLocation();
	RealStartVel = StartVelocity;
	
	float StartPosX = CameraTransform.GetLocation().X;
	float StartPosY = CameraTransform.GetLocation().Y;
	float StartPosZ = CameraTransform.GetLocation().Z;
	
	StartPos = { StartPosX, StartPosY, StartPosZ };
	
	StartVel = 
		FVector::VectorPlaneProject(
			RealStartVel, 
			CameraTransform.GetUnitAxis(EAxis::Y)).GetSafeNormal() * RealStartVel.Length();
	
	AlignmentTime = InAlignmentTime;
	bAligned = false;
	bHasValidData = true;
}

FVector FBSProjectileAlignment::EvaluatePosition(const float TimeAlive, const FVector& GravityAcceleration)
{
	const FVector AlignedPos = StartPos + (StartVel + 0.5f * TimeAlive * GravityAcceleration) * TimeAlive;
	const FVector UnalignedPos = RealStartPos + (RealStartVel + 0.5f * TimeAlive * GravityAcceleration) * TimeAlive;
	
	if (TimeAlive >= AlignmentTime) bAligned = true;
	
	const float Alpha = FMath::SmoothStep(0.f, 1.f, TimeAlive / AlignmentTime);
	return Slerp(UnalignedPos, AlignedPos, Alpha);
}

ABSProjectileBase::ABSProjectileBase(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	OverlapComponent = CreateDefaultSubobject<USphereComponent>("OverlapComponent");
	SetRootComponent(OverlapComponent);
	OverlapComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	OverlapComponent->SetCollisionObjectType(ECC_Pawn);
	ProjectileMovementComponent = 
		CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->SetUpdatedComponent(OverlapComponent);
}

ABSProjectileBase* ABSProjectileBase::SpawnProjectile(
	AActor* InOwnerActor,
	const TSubclassOf<ABSProjectileBase> ProjectileClass,
	const FTransform& InSpawnTransform,
	const FTransform& InCameraTransform)
{
	APawn* Instigator = Cast<APawn>(InOwnerActor);
	
	ABSProjectileBase* ProjectileInstance = InOwnerActor->GetWorld()->SpawnActorDeferred<ABSProjectileBase>(
		ProjectileClass, 
		InSpawnTransform, 
		InOwnerActor, 
		Instigator, 
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	
	ProjectileInstance->ProjectileAlignment = 
		FBSProjectileAlignment(
			InSpawnTransform, 
			InSpawnTransform.TransformVector({1, 0, 0}) * ProjectileInstance->ProjectileMovementComponent->InitialSpeed, 
			InCameraTransform, 
			0.35);
	
	ProjectileInstance->FinishSpawning(InSpawnTransform);
	
	return ProjectileInstance;
}

ABSProjectileBase* ABSProjectileBase::SpawnProjectile(AActor* InOwnerActor,
	const TSubclassOf<ABSProjectileBase> ProjectileClass, const FTransform& InSpawnTransform)
{
	APawn* Instigator = Cast<APawn>(InOwnerActor);
	
	ABSProjectileBase* ProjectileInstance = InOwnerActor->GetWorld()->SpawnActorDeferred<ABSProjectileBase>(
		ProjectileClass, 
		InSpawnTransform, 
		InOwnerActor, 
		Instigator, 
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	
	ProjectileInstance->FinishSpawning(InSpawnTransform);
	
	return ProjectileInstance;
}

void ABSProjectileBase::Tick(const float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	TimeAlive += DeltaSeconds;

	if (!IsActorBeingDestroyed() && ProjectileAlignment.GetIsValid() && !ProjectileAlignment.GetIsAligned())
	{
		const FVector newPosition = ProjectileAlignment.EvaluatePosition(TimeAlive, {0,0,ProjectileMovementComponent->GetGravityZ()});
		ProjectileMovementComponent->Velocity = (newPosition - OverlapComponent->GetComponentLocation()) / DeltaSeconds;
	}
}

void ABSProjectileBase::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	if (!OtherActor || !GetOwner() || !GetAbilitySystemComponent()) return;
	if (HitActors.Contains(OtherActor)) return;
	if (const IAbilitySystemInterface* AscInterface = Cast<IAbilitySystemInterface>(OtherActor); 
		AscInterface && !OtherActor->IsA(StaticClass()))
	{
		if (AscInterface->GetAbilitySystemComponent())
		{
			bool bHit = false;
			
			bool bMatchingFaction = UBSAbilityLibrary::HasMatchingFaction(GetAbilitySystemComponent(),AscInterface->GetAbilitySystemComponent());
			
			if (bHitEnemies && !bMatchingFaction)
			{
				bHit = true;
			}
			
			if (bHitFriendlies && bMatchingFaction)
			{
				bHit = true;
			}
			
			if (bHit)
			{
				HitActors.Add(OtherActor);
				CurrentHits++;
				
				if (GetOwner()->HasAuthority())
				{
					GetAbilitySystemComponent()->BP_ApplyGameplayEffectToTarget(EffectClass, 
						AscInterface->GetAbilitySystemComponent(), 1, GetAbilitySystemComponent()->MakeEffectContext());
				}
				
				if (CurrentHits >= MaxHits)
				{
					Destroy();
				}
			}
		}
	}
}

UAbilitySystemComponent* ABSProjectileBase::GetAbilitySystemComponent() const
{
	if (const IAbilitySystemInterface* AbilitySystemInterface = Cast<IAbilitySystemInterface>(GetOwner()))
	{
		return AbilitySystemInterface->GetAbilitySystemComponent();
	}
	return nullptr;
}

