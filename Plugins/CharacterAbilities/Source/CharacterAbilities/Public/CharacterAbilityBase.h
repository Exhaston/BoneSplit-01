// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "CharacterAbilityBase.generated.h"

class UCooldownGameplayEffect;

//Used by the buffer component
UENUM(BlueprintType)
enum EAbilityBufferMode
{
	EABT_None UMETA(DisplayName = "None"),
	EABT_Independent UMETA(DisplayName = "Default", ToolTip = "Many Independent buffers may be active at the same time"),
	EABT_Unique UMETA(DisplayName = "Unique", ToolTip = "Only one buffer of this type can be active, and it will replace the old."),
};

UENUM(BlueprintType)
enum EActivationGroup
{
	EAG_Independent UMETA(DisplayName = "Independent", ToolTip = "Will run independantly of other abilities."),
	EAG_ExclusiveReplaceable UMETA(DisplayName = "Exclusive Replaceable", ToolTip = "Will be canceled in favor of newer Blocking or Replaceable abilities."),
	EAG_ExclusiveBlocking UMETA(DisplayName = "Exclusive Blocking", ToolTip = "Will block all other exclusive abilities (Blocking and Replaceable) from activating."),
};

/**                                                                   
 * 
 */
UCLASS(DisplayName="Character Ability Base", HideCategories=("Costs", "Input", "Advanced"))
class CHARACTERABILITIES_API UCharacterAbilityBase : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	
	UCharacterAbilityBase();
	
	virtual void OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	
	virtual bool CanActivateAbility(
		const FGameplayAbilitySpecHandle Handle, 
		const FGameplayAbilityActorInfo* ActorInfo, 
		const FGameplayTagContainer* SourceTags = nullptr, 
		const FGameplayTagContainer* TargetTags = nullptr, 
		FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	
	virtual bool CommitAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, FGameplayTagContainer* OptionalRelevantTags = nullptr) override;
	
	virtual void CommitExecute(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
	UFUNCTION(BlueprintPure)
	ACharacter* GetCharacterAvatar() const;
	
	virtual const FGameplayTagContainer* GetCooldownTags() const override;
	
	virtual void ApplyCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const override;
	
	virtual bool CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	
	virtual float GetCostModifier() const;
	
	virtual float GetCooldownModifier() const;
	
	virtual void ApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const override;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Buffering")
	TEnumAsByte<EAbilityBufferMode> BufferChannel;
	
protected:
	
	//How ability handles other abilities being active.
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="ActivationGroup")
	TEnumAsByte<EActivationGroup> ActivationGroup = EAG_ExclusiveBlocking;
	
	//If Activation group is replaceable, this can allow for self cancellations before reactivating.
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	bool bCanCancelSelf = false;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	bool bApplyCostOnEnd = false;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta=(ClampMin=0, ForceInlineRow))
	TMap<FGameplayAttribute, float> AbilityCost;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Cooldown")
	bool bApplyCooldownOnEnd = false;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Cooldown")
	FScalableFloat CooldownDuration;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta=(ClampMin=1))
	int32 AbilityUses = 1;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta=(Categories="Cooldown."))
	FGameplayTagContainer CooldownTags;
	
	virtual bool IsMovementModeCompatible() const;
	virtual bool MovementRequirementsMet() const;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TSet<TEnumAsByte<EMovementMode>> CompatibleMovementModes = 
		{ MOVE_Walking, MOVE_Flying, MOVE_Falling, MOVE_NavWalking, MOVE_None, MOVE_Swimming, MOVE_Custom};
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TArray<uint8> CompatibleCustomMovementModes; 
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta=(ClampMin=-1, Units=centimeters))
	float MaxAllowedVelocity = -1;
	      
	UPROPERTY(Transient)
	TWeakObjectPtr<ACharacter> CharacterAvatar;
	
	UPROPERTY(Transient)
	FGameplayTagContainer TempCooldownTags;
	
};
