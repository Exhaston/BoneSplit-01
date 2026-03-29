// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "AnimInstance/MMobAnimInstance.h"

#include "KismetAnimationLibrary.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

void UMMobAnimInstance::NativeInitializeAnimation()
{
	AnimationPlaybackOffset = FMath::RandRange(0.f, 1.f); 
	
	Super::NativeInitializeAnimation();
}

void UMMobAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
	
	OwnerCharacter = Cast<ACharacter>(TryGetPawnOwner());
	
	if (OwnerCharacter)
	{
		CharMovementComp = OwnerCharacter->GetCharacterMovement();
		bReadyForUpdate = true;
	}
}

void UMMobAnimInstance::NativeUpdateAnimation(const float DeltaSeconds)
{
	if (!bReadyForUpdate || !OwnerCharacter || !CharMovementComp) return;
	
	Super::NativeUpdateAnimation(DeltaSeconds);

#if WITH_EDITOR
	
	if (OwnerCharacter->GetWorld()->IsGameWorld())
	{
		bIsGrounded = !CharMovementComp->IsFalling();
	}
	
#else

	bIsGrounded = !CharMovementComp->IsFalling();
	
#endif
	
	FRotator AimRotation = OwnerCharacter->GetBaseAimRotation();
	
	AimRotation -= OwnerCharacter->GetActorRotation();
	
	AimRotation.Pitch = FRotator::NormalizeAxis(AimRotation.Pitch);
	AimRotation.Pitch *= -1;
	
	CharacterAimRotationPitch = AimRotation.Pitch;
	CharacterAimRotationYaw = (AimRotation.Yaw >= YawAimRange.GetLowerBoundValue() && AimRotation.Yaw <= YawAimRange.GetUpperBoundValue()) ? AimRotation.Yaw : 0;
	
	DefaultMaxMoveSpeed = CharMovementComp->MaxWalkSpeed;
	
	FVector CurrentVelocity = CharMovementComp->Velocity;
	
	GravityMagnitude = CurrentVelocity.Z;
	CurrentVelocity.Z = 0;
	
	if (OwnerCharacter->IsLocallyControlled() && OwnerCharacter->IsPlayerControlled())
	{
		CurrentVelocity = 
			CharMovementComp->GetCurrentAcceleration().GetClampedToMaxSize(CharMovementComp->GetMaxSpeed());
	}
	
	WorldSpaceMoveVelocity = CurrentVelocity;
	MoveSpeedMagnitude = CurrentVelocity.Length();
	LocalSpaceMoveVelocity = OwnerCharacter->GetActorTransform().TransformVector(CurrentVelocity);
	
	if (!FMath::IsNearlyZero(DefaultMaxMoveSpeed))
	{
		CurrentSpeedPercent = (MoveSpeedMagnitude / DefaultMaxMoveSpeed) * 100.f;
	}
	
	const float TargetDirection = 
		UKismetAnimationLibrary::CalculateDirection(WorldSpaceMoveVelocity, OwnerCharacter->GetActorRotation());
	
	MoveDirectionAngle = 
		FMath::RoundToFloat(TargetDirection / VelocityDirectionStepSize) * VelocityDirectionStepSize;
}

void UMMobAnimInstance::BP_OnMobDied_Implementation()
{
}
