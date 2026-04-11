// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Player/Abilities/BSPlayerAbilityBase_AutoVault.h"

#include "AbilitySystemComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Tasks/AbilityTask_TickTask.h"

UBSPlayerAbilityBase_AutoVault::UBSPlayerAbilityBase_AutoVault()
{
	BufferChannel = EABT_None;
	ActivationGroup = EAG_Independent;
}

void UBSPlayerAbilityBase_AutoVault::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo,
                                                   const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);
	if (ActorInfo->AbilitySystemComponent.IsValid())
	{
		ActorInfo->AbilitySystemComponent->TryActivateAbility(Spec.Handle);
	}

}

void UBSPlayerAbilityBase_AutoVault::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	if (CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		bReady = !GetCharacterAvatar()->GetCharacterMovement()->IsFalling();
		GetCharacterAvatar()->MovementModeChangedDelegate.AddDynamic(this, &UBSPlayerAbilityBase_AutoVault::OnCharacterMovementModeChanged);
		UAbilityTask_TickTask* TickTask = UAbilityTask_TickTask::AbilityTaskOnTick(this, FName("TickVaultTask"));
		TickTask->OnTick.AddDynamic(this, &UBSPlayerAbilityBase_AutoVault::OnAbilityTick);
		TickTask->ReadyForActivation();
		return;
	}
	
	CancelAbility(Handle, ActorInfo, ActivationInfo, true);
}

void UBSPlayerAbilityBase_AutoVault::OnCharacterMovementModeChanged(ACharacter* Character,
	EMovementMode NewMovementMode, uint8 CustomIndex)
{
	if (GetCharacterAvatar()->GetCharacterMovement()->IsMovingOnGround())
	{
		bReady = true;
	}
}

void UBSPlayerAbilityBase_AutoVault::OnAbilityTick(float DeltaSeconds)
{
	FVector LedgeTop, LedgeNormal;
	if (bReady && TryDetectVault(LedgeTop, LedgeNormal))
	{
		const float FeetZ       = GetCharacterAvatar()->GetActorLocation().Z
								  - GetCharacterAvatar()->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
		const float HeightDiff  = LedgeTop.Z - FeetZ;
		
		const float GravityAbs  = FMath::Abs(GetWorld()->GetGravityZ() * 2);

		// Physics-accurate Z to just clear the ledge
		const FVector Launch = FVector(
			0,
			0,
			FMath::Sqrt(2.f * GravityAbs * (HeightDiff + GetCharacterAvatar()->GetCapsuleComponent()->GetScaledCapsuleHalfHeight()))
		);

		bReady = false;
		GetCharacterAvatar()->GetCharacterMovement()->SetMovementMode(MOVE_Falling);
		GetCharacterAvatar()->LaunchCharacter(Launch, false, true);
	}
}

bool UBSPlayerAbilityBase_AutoVault::TryDetectVault(FVector& OutLedgeTop, FVector& OutLedgeNormal) const
{
    ACharacter* Char = GetCharacterAvatar();
    if (!Char) return false;

    const FVector MoveInput = Char->GetCharacterMovement()->GetLastInputVector();
    if (FVector::DotProduct(MoveInput.GetSafeNormal2D(), Char->GetActorForwardVector()) < 0.7f)
        return false;

    const UCapsuleComponent* Capsule = Char->GetCapsuleComponent();
    const float CapsuleRad  = Capsule->GetScaledCapsuleRadius();
    const float CapsuleHalf = Capsule->GetScaledCapsuleHalfHeight();
    const FVector CharFwd   = Char->GetActorForwardVector();
    const FVector CharLoc   = Char->GetActorLocation();
    const float FeetZ       = CharLoc.Z - CapsuleHalf;

    TArray<TEnumAsByte<EObjectTypeQuery>> VaultableObjects;
    VaultableObjects.Add(UEngineTypes::ConvertToObjectType(ECC_WorldStatic));

    // ── 1. Forward obstacle check (unchanged, keeps your wall-facing logic) ──
    FHitResult WallHit;
    const bool bHitWall = UKismetSystemLibrary::SphereTraceSingleForObjects(
        GetWorld(), CharLoc, CharLoc + CharFwd * CapsuleRad,
        CapsuleRad, VaultableObjects,
        false, {Char}, EDrawDebugTrace::None, WallHit, true);

    if (!bHitWall) return false;

    const FVector NormalFlat = FVector(WallHit.ImpactNormal.X, WallHit.ImpactNormal.Y, 0.f).GetSafeNormal();
    if (WallHit.ImpactNormal.Z > 0.7f)                                          return false; // floor
    if (FVector::DotProduct(NormalFlat, -CharFwd) < 0.2f)                       return false; // behind/beside

    // ── 2. Project probe origin onto the obstacle surface ──
    //    Use the wall impact point pushed forward slightly so the sphere
    //    lands *on top of* the obstacle rather than in front of it.
    const FVector ProbeXY  = WallHit.ImpactPoint - NormalFlat * CapsuleRad * 0.5f;

    // Sweep from max vault height downward
    const float   SweepRadius = CapsuleRad * 0.5f;   // smaller = more precise landing point
    const FVector SweepStart  = FVector(ProbeXY.X, ProbeXY.Y, FeetZ + 144.f);
    const FVector SweepEnd    = FVector(ProbeXY.X, ProbeXY.Y, FeetZ);

    FHitResult LedgeHit;
    const bool bFoundLedge = UKismetSystemLibrary::SphereTraceSingleForObjects(
        GetWorld(), SweepStart, SweepEnd,
        SweepRadius, VaultableObjects,
        false, {Char}, EDrawDebugTrace::None, LedgeHit, true);

    if (!bFoundLedge) return false;

    // ── 3. Validate surface ──
    if (FVector::DotProduct(LedgeHit.ImpactNormal, FVector::UpVector) < 0.6f)  return false; // too steep

    const float ObjectHeight = LedgeHit.ImpactPoint.Z - FeetZ;
    if (ObjectHeight < Char->GetCharacterMovement()->MaxStepHeight)             return false; // just a step
    if (ObjectHeight > 144.f)                                                   return false; // too tall

    // ── 4. Headroom check at landing spot ──
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(Char);

    const FVector ClearStart = LedgeHit.ImpactPoint + FVector(0, 0, 5.f);
    const FVector ClearEnd   = ClearStart + FVector(0, 0, CapsuleHalf * 2.f + 20.f);

    FHitResult ClearHit;
    if (GetWorld()->LineTraceSingleByChannel(ClearHit, ClearStart, ClearEnd, ECC_WorldStatic, Params))
        return false; // no headroom

    OutLedgeTop    = LedgeHit.ImpactPoint;
    OutLedgeNormal = WallHit.ImpactNormal;
    return true;
}
