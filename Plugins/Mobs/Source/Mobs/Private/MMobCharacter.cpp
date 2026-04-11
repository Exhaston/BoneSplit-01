// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "MMobCharacter.h"

#include "MMobController.h"
#include "MMobMovementComponent.h"
#include "NavigationSystem.h"
#include "Components/CapsuleComponent.h"
#include "Patrol/MMobPatrolComponent.h"

AMMobCharacter::AMMobCharacter(const FObjectInitializer& ObjectInitializer) :
Super(ObjectInitializer.SetDefaultSubobjectClass<UMMobMovementComponent>(CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = true;
	AIControllerClass = AMMobController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	
	
	GetMesh()->SetReceivesDecals(false);
	GetMesh()->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -64));
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::Type::QueryOnly);
	GetCapsuleComponent()->SetCapsuleHalfHeight(64);
	GetCapsuleComponent()->SetCapsuleRadius(32);
	
	/* Replication */
	bAlwaysRelevant = true;
	SetNetUpdateFrequency(64);
	bReplicates = true;
	bUseControllerRotationYaw = false;
	
	//More precise rotations
	GetReplicatedMovement_Mutable().RotationQuantizationLevel = ERotatorQuantization::ShortComponents;
}

void AMMobCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
	if (AMMobController* MobController = Cast<AMMobController>(NewController))
	{
		MobController->SetMoveAcceptanceRadius(MoveToThreatAcceptanceRadius);
		
		MobController->GetOnMobTickDelegate().AddUObject(this, &AMMobCharacter::NativeOnMobTick);
		MobController->GetThreatComponent()->GetOnTargetChangedDelegate().AddUObject(this, &AMMobCharacter::OnTargetChanged);
		MobController->GetOnFocusTargetChangedDelegate().AddUObject(this, &AMMobCharacter::OnFocusTargetChanged);
		
		InitNavigation();
	}
}

void AMMobCharacter::InitNavigation()
{
	//Cursed but the navigation system is shit
	TWeakObjectPtr WeakThis(this);
	GetWorldTimerManager().SetTimer(MobNavMeshTimer, [WeakThis]()
	{
		if (!WeakThis.IsValid())
			return;

		ThisClass* Self = WeakThis.Get();

		UWorld* World = Self->GetWorld();
		if (!World || !World->bIsWorldInitialized)
			return;

		const UNavigationSystemV1* NavSys = 
			FNavigationSystem::GetCurrent<UNavigationSystemV1>(World);
			
		if (!NavSys || !NavSys->IsWorldInitDone())
			return;
			
		Self->GetWorldTimerManager().ClearTimer(Self->MobNavMeshTimer);
		Self->OnNavigationReady();

	}, 1.0f, true);
}

void AMMobCharacter::UnPossessed()
{
	if (AMMobController* MobController = GetController<AMMobController>())
	{
		MobController->GetOnMobTickDelegate().RemoveAll(this);
		MobController->GetThreatComponent()->GetOnTargetChangedDelegate().RemoveAll(this);
		MobController->GetOnFocusTargetChangedDelegate().RemoveAll(this);
	}
		
	Super::UnPossessed();
}

void AMMobCharacter::LaunchCharacter(FVector LaunchVelocity, bool bXYOverride, bool bZOverride)
{
	GetCharacterMovement()->StopMovementImmediately();
	Super::LaunchCharacter(LaunchVelocity, bXYOverride, bZOverride);
}

void AMMobCharacter::OnFocusTargetChanged(AActor* NewFocus)
{
	GetCharacterMovement()->bUseControllerDesiredRotation = IsValid(NewFocus);
	GetCharacterMovement()->bOrientRotationToMovement = !IsValid(NewFocus);
	
	OnFocusTargetChangedDelegate.Broadcast(NewFocus);
}

void AMMobCharacter::OnTargetChanged(AActor* OldTarget, AActor* NewTarget)
{
	if (NewTarget)
	{
		GetMobController()->GetMobPatrolComponent()->StopPatrol();
	}
	else if (MobPatrolPath)
	{
		GetMobController()->GetMobPatrolComponent()->StartPatrol(MobPatrolPath);
	}
	
	OnTargetChangedDelegate.Broadcast(OldTarget, NewTarget);
}

void AMMobCharacter::OnNavigationReady()
{
	if (MobPatrolPath && !GetMobController()->GetThreatComponent()->GetHighestThreat())
	{
		GetMobController()->GetMobPatrolComponent()->StartPatrol(MobPatrolPath);
	}
}

void AMMobCharacter::NativeOnMobTick(const float DeltaSeconds)
{
	BP_OnMobTick(DeltaSeconds);
}

void AMMobCharacter::BP_OnMobTick_Implementation(float DeltaSeconds)
{
}

AMMobController* AMMobCharacter::GetMobController()
{
	return GetController<AMMobController>();
}

UMThreatComponent* AMMobCharacter::GetThreatComponent()
{
	if (GetMobController())
	{
		return GetMobController()->GetThreatComponent();
	}
	return nullptr;
}

