// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Actors/Predictables/BSProjectileBase.h"

#include "AbilitySystemComponent.h"
#include "Components/SphereComponent.h"
#include "Components/AbilitySystem/BSAbilityLibrary.h"
#include "Components/AbilitySystem/EffectBases/BSGameplayEffect.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Statics/BSMathLibrary.h"

FBSProjectileAlignment::FBSProjectileAlignment(
	AActor* Owner,
	const FTransform& SpawnTransform,
	const FTransform& CameraTransform)
{

	const FVector CameraLocation = CameraTransform.GetLocation();
	CameraForward = CameraTransform.GetUnitAxis(EAxis::X);
	const FVector SpawnLocation = SpawnTransform.GetLocation();
	StartPos = SpawnLocation;
	
	const float ProjectedDistance = FVector::DotProduct(SpawnLocation - CameraLocation, CameraForward);
	
	FVector StartPoint = CameraLocation + CameraForward * ProjectedDistance;
	
	FCollisionObjectQueryParams CollisionObjectQueryParams; 
	CollisionObjectQueryParams.AddObjectTypesToQuery(ECC_Pawn);
	CollisionObjectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);
	
	FCollisionQueryParams CollisionQueryParams;
	CollisionQueryParams.AddIgnoredActor(Owner);

	const FVector TraceEnd = StartPoint + CameraForward * 2000;
	if (FHitResult Hit; Owner->GetWorld()->LineTraceSingleByObjectType(
		Hit, 
		StartPoint, 
		TraceEnd, 
		CollisionObjectQueryParams, 
		CollisionQueryParams))
	{
		TargetPos = Hit.ImpactPoint;
	}
	else
	{
		TargetPos = TraceEnd;
	}
	
	bAligned = false;
	bHasValidData = true;
}

ABSProjectileBase::ABSProjectileBase(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	OverlapComponent = CreateDefaultSubobject<USphereComponent>("OverlapComponent");
	SetRootComponent(OverlapComponent);
	
	ProjectileMovementComponent = 
		CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->SetUpdatedComponent(OverlapComponent);
}

void ABSProjectileBase::BeginPlay()
{
	Super::BeginPlay();
	
	if (ProjectileAlignment.GetIsValid())
	{
		ProjectileMovementComponent->StopMovementImmediately();
		ProjectileMovementComponent->SetComponentTickEnabled(false);
	}
}

void ABSProjectileBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	SetCollisionProfiles();
}

void ABSProjectileBase::SetCollisionProfiles()
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

TArray<ABSProjectileBase*> ABSProjectileBase::SpawnProjectiles(
	AActor* InOwnerActor, 
	const TSubclassOf<ABSProjectileBase> 
	ProjectileClass, 
	const FTransform& InSpawnTransform,
	const int32 NumProjectiles,
	const float SpreadDegrees)
{
	TArray<ABSProjectileBase*> Projectiles;
	
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
    	
		ABSProjectileBase* ProjectileInstance = World->SpawnActorDeferred<ABSProjectileBase>(
		ProjectileClass, 
		NewProjectileTransform, 
		InOwnerActor, 
		NewInstigator, 
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	
		ProjectileInstance->FinishSpawning(NewProjectileTransform);
	}
	return Projectiles;
}

void ABSProjectileBase::Tick(const float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	if (IsActorBeingDestroyed()) return;
	
	TimeAlive += DeltaSeconds;
	
	if (!ProjectileAlignment.GetIsValid() || ProjectileAlignment.GetIsAligned()) return;

	const float Alpha = FMath::Clamp(GetGameTimeSinceCreation() / ProjectileAlignment.GetAlignmentTime(ProjectileMovementComponent->InitialSpeed), 0.f, 1.f);
	const FVector NewPos = FMath::Lerp(ProjectileAlignment.StartPos, ProjectileAlignment.TargetPos, Alpha);
	
	SetActorLocation(NewPos, true);
	
	if (Alpha >= 1.f)
	{
		ProjectileAlignment.bAligned = true;
		
		ProjectileMovementComponent->Velocity =
			ProjectileAlignment.CameraForward * ProjectileMovementComponent->InitialSpeed;
		
		ProjectileMovementComponent->SetComponentTickEnabled(true);
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

void ABSProjectileBase::NotifyHit(
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
			const bool bMatchingFaction = UBSAbilityLibrary::HasMatchingFaction(
				GetAbilitySystemComponent(),AscInterface->GetAbilitySystemComponent());
			
			if (bHitFriendlies && bMatchingFaction || bHitEnemies && !bMatchingFaction)
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

