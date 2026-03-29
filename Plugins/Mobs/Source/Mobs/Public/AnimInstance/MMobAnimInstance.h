// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "MMobAnimInstance.generated.h"

class UCharacterMovementComponent;
class IMAnimInfoInterface;
/**
 * 
 */
UCLASS(DisplayName="Mob Anim Instance", Blueprintable, BlueprintType)
class MOBS_API UMMobAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	
	virtual void NativeInitializeAnimation() override;
	
	virtual void NativeBeginPlay() override;
	
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	
	UFUNCTION(BlueprintNativeEvent, DisplayName="OnMobDied")
	void BP_OnMobDied();
	
	/* Thread safe Getters */
	
	UFUNCTION(BlueprintPure, meta=(BlueprintThreadSafe), Category="Mob Anim Instance")
	float GetMoveSpeedPercentage()
	{
		return CurrentSpeedPercent;
	}

	UFUNCTION(BlueprintPure, meta=(BlueprintThreadSafe), Category="Mob Anim Instance")
	float GetMoveDirectionAngle()
	{
		return MoveDirectionAngle;
	}

	UFUNCTION(BlueprintPure, meta=(BlueprintThreadSafe), Category="Mob Anim Instance")
	float GetMoveSpeedMagnitude()
	{
		return MoveSpeedMagnitude;
	}
	
	UFUNCTION(BlueprintPure, meta=(BlueprintThreadSafe), Category="Mob Anim Instance")
	float GetDefaultMaxMoveSpeed()
	{
		return DefaultMaxMoveSpeed;
	}
	
	UFUNCTION(BlueprintPure, meta=(BlueprintThreadSafe), Category="Mob Anim Instance")
	float GetGravityMagnitude()
	{
		return GravityMagnitude;
	}
	
	UFUNCTION(BlueprintPure, meta=(BlueprintThreadSafe), Category="Mob Anim Instance")
	FVector GetWorldSpaceMoveVelocity()
	{
		return WorldSpaceMoveVelocity;
	}
	
	UFUNCTION(BlueprintPure, meta=(BlueprintThreadSafe), Category="Mob Anim Instance")
	FVector GetLocalSpaceMoveVelocity()
	{
		return LocalSpaceMoveVelocity;
	}
	
	UFUNCTION(BlueprintPure, meta=(BlueprintThreadSafe), Category="Mob Anim Instance")
	bool GetIsGrounded()
	{
		return bIsGrounded;
	}
	
	UFUNCTION(BlueprintPure, meta=(BlueprintThreadSafe), Category="Mob Anim Instance")
	bool GetIsFalling()
	{
		return !bIsGrounded;
	}
	
	UFUNCTION(BlueprintPure, meta=(BlueprintThreadSafe), Category="Mob Anim Instance")
	bool GetIsMoving()
	{
		return !FMath::IsNearlyZero(CurrentSpeedPercent);
	}
	
	UFUNCTION(BlueprintPure, meta=(BlueprintThreadSafe), Category="Mob Anim Instance")
	float GetAimPitch()
	{
		return CharacterAimRotationPitch;
	}
	
	UFUNCTION(BlueprintPure, meta=(BlueprintThreadSafe), Category="Mob Anim Instance")
	float GetAimYaw()
	{
		return CharacterAimRotationYaw;
	}
	
protected:
	
	//Current speed percent
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta=(ClampMin=0, Units=Percent), Category="Mob Anim Instance")
	float CurrentSpeedPercent = 0.f;
	
	//Current max movement speed, typically characters max walk speed.
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta=(ClampMin=0, Units=Centimeters), Category="Mob Anim Instance")
	float DefaultMaxMoveSpeed = 450.f;
	
	//The rotation the character is aiming towards. 
	//For player controller this is control rotation, for mobs it would be their focus direction.
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta=(ClampMin=-180, ClampMax=180, Units=Degrees), Category="Mob Anim Instance")
	float CharacterAimRotationYaw = 0.f;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta=(ClampMin=-90, ClampMax=90, Units=Degrees), Category="Mob Anim Instance")
	float CharacterAimRotationPitch = 0.f;
	
	//The step size to snap to on the move direction angle computations. 
	//This helps avoid awkward blending between directions.
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta=(ClampMin=0, Units=Degrees), Category="Mob Anim Instance")
	float VelocityDirectionStepSize = 10;
	
	//-180 backward, -90 left, 0 Forward, 90 Right 180 backward
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta=(ClampMin=-180, ClampMax=180, Units=Degrees), Category="Mob Anim Instance")
	float MoveDirectionAngle = 0;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta=(Units=Degrees), Category="Mob Anim Instance")
	FFloatRange YawAimRange = FFloatRange(-90, 90);
	
	//Owner character of this anim instance. Set during begin play and blocks tick until it's ready.
	UPROPERTY(Transient, BlueprintReadOnly, VisibleAnywhere, Category="Mob Anim Instance")
	TObjectPtr<ACharacter> OwnerCharacter = nullptr;
	
	//Owner Movement component
	UPROPERTY(Transient, BlueprintReadOnly, VisibleAnywhere, Category="Mob Anim Instance")
	TObjectPtr<UCharacterMovementComponent> CharMovementComp = nullptr;
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category="Mob Anim Instance")
	float AnimationPlaybackOffset = 0.f;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Mob Anim Instance")
	bool bIsGrounded = true;
	
	FVector WorldSpaceMoveVelocity = FVector::ZeroVector;
	FVector LocalSpaceMoveVelocity = FVector::ZeroVector;
	float MoveSpeedMagnitude = 0.f;
	float GravityMagnitude = 0.f;
	bool bReadyForUpdate = false;
};
