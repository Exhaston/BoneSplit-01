// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Animation/AnimInstance.h"
#include "BoneSplit/BoneSplit.h"
#include "BSAnimInstance.generated.h"

class UAbilitySystemComponent;
class IAbilitySystemInterface;
/**
 * 
 */
UCLASS(DisplayName="Anim Instance Base", Category="BoneSplit")
class BONESPLIT_API UBSAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	
	virtual void NativeInitializeAnimation() override;
	
	
	virtual void InitializeAbilitySystemComponent(UAbilitySystemComponent* InAbilitySystemComponent);
	
	UFUNCTION(BlueprintNativeEvent)
	void OnAbilitySystemReady(UAbilitySystemComponent* OwnerAbilitySystemComponent);
	
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	
	//Fetches the current AbilitySystemComponent or caches a new one from the Pawn.
	UFUNCTION(BlueprintPure)
	UAbilitySystemComponent* GetAbilitySystemComponent();
	
	UPROPERTY(Transient)
	TWeakObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
	
	UPROPERTY()
	ACharacter* CharacterOwner;
	
	//Gets a copy of the current gameplay tags the Owning Ability System.
	//See OnGameplayTagRemoved and OnGameplayTagAdded for callbacks
	UFUNCTION(BlueprintPure, meta=(BlueprintThreadSafe), Category="Bone Split")
	FGameplayTagContainer GetGameplayTagContainer() const { return OwnedGameplayTags; }
	
	UFUNCTION()
	void NativeOnTagEvent(const FGameplayTag Tag, int32 Count);
	                                                                                 
	UFUNCTION(BlueprintNativeEvent, Category="Bone Split", DisplayName="OnGameplayTagAdded")
	void BP_OnGameplayTagAdded(const FGameplayTag& Tag, const int32& Count);
	
	UFUNCTION(BlueprintNativeEvent, Category="Bone Split", DisplayName="OnGameplayTagRemoved")
	void BP_OnGameplayTagRemoved(const FGameplayTag& Tag, const int32& Count);
	
	UFUNCTION(BlueprintNativeEvent, Category="Bone Split", DisplayName="OnGameplayTagChanged")
	void BP_OnGameplayTagChanged(const FGameplayTag& Tag, const int32& Count);
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FGameplayTagContainer OwnedGameplayTags;
	
	UPROPERTY()
	bool bInitialized = false;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Bone Split", BlueprintGetter=GetAnimationOffset)
	float NormalizedAnimationOffset;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Bone Split", BlueprintGetter=GetVelocityPercentage,
		meta=(Units=Percent, ClampMin=0))
	float VelocityPercentage = 100;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta=(Units=Degrees))
	float VelocityDirectionStepSize = 5;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Bone Split", BlueprintGetter=GetVelocityDirection,
		meta=(Units=Degrees, ClampMin=-180, ClampMax=180))
	float VelocityDirection;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Bone Split", BlueprintGetter=GetGravityVelocity,
	meta=(Units=Centimeters))
	float GravityVelocity;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Bone Split", BlueprintGetter=GetIsFalling)
	bool IsFalling;
	
	//If set to true, will use desired velocity instead of actual velocity. 
	//If stuck to a wall this will still register the velocity even though the character is standing still.
	//Turn off to revert to default velocity magnitude.
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	bool bUseDesiredVelocity = true;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float LookInterpSpeed = 15;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta=(Categories="WeaponType"))
	FGameplayTag WeaponTypeTag = BSTags::WeaponType_SwordNBoard;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Bone Split", BlueprintGetter=GetAimRotation)
	FRotator AimRotation;
	
	//Returns a random value between 0 - 1 for animation offsets. 
	//Used mainly to offset the cycles of looping animations.
	UFUNCTION(BlueprintPure, meta=(BlueprintThreadSafe), Category="Bone Split")
	float GetAnimationOffset() const { return NormalizedAnimationOffset; }
	
	//Returns the current speed percentage of the max walk speed. Can go above 100 if speed is temporarily increased
	UFUNCTION(BlueprintPure, meta=(BlueprintThreadSafe), Category="Bone Split")
	float GetVelocityPercentage() const { return VelocityPercentage; }
	                          
	//Returns the angle of velocity from forward. -180 - 180 = Backward, -90 - 90 = Strafe.
	UFUNCTION(BlueprintPure, meta=(BlueprintThreadSafe), Category="Bone Split")
	float GetVelocityDirection() const { return VelocityPercentage; }
	
	UFUNCTION(BlueprintPure, meta=(BlueprintThreadSafe), Category="Bone Split")
	float GetGravityVelocity() const { return GravityVelocity; }
	
	UFUNCTION(BlueprintPure, meta=(BlueprintThreadSafe), Category="Bone Split")
	void GetBlendSpaceProperties(float& OutHorizontal, float& OutVertical, float& OutOffset) const
	{ OutHorizontal = VelocityDirection; OutVertical = VelocityPercentage; OutOffset = NormalizedAnimationOffset; }
	
	UFUNCTION(BlueprintPure, meta=(BlueprintThreadSafe), Category="Bone Split")
	bool GetIsFalling() const { return IsFalling; }
	
	//Returns the current rotation from the Pawns Base Aim Rotation(Replicated).
	//Used to apply additive animations based on camera directions.
	UFUNCTION(BlueprintPure, meta=(BlueprintThreadSafe), Category="Bone Split")
	FRotator GetAimRotation() const { return AimRotation; }
};
