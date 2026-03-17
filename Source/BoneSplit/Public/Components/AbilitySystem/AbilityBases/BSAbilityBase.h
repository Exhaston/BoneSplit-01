// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CharacterAbilityBase.h"
#include "IconThumbnailInterface.h"
#include "Abilities/GameplayAbility.h"
#include "BoneSplit/BoneSplit.h"
#include "BSAbilityBase.generated.h"

class ABSProjectileBase;
class UBSAbilitySystemComponent;
class ABSPredictableActor;

#define COMMIT_ABILITY(Handle, ActorInfo, ActivationInfo)   \
if (!CommitAbility(Handle, ActorInfo, ActivationInfo))      \
{                                                           \
	CancelAbility(Handle, ActorInfo, ActivationInfo, true); \
	return;                                                 \
}
#define CANCEL_ABILITY() \
CancelAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true); return;

UENUM(BlueprintType)
enum EBSAbilityInputID                
{   
	EAII_None = 0 UMETA(DisplayName = "None"),
	EAII_Special = 1 UMETA(DisplayName = "Special"),                 
	EAII_Head = 2 UMETA(DisplayName = "Head"),
	EAII_Legs = 3 UMETA(DisplayName = "Legs"),
	EAII_Soul = 4 UMETA(DisplayName = "Soul"),
	EAII_Primary = 5 UMETA(DisplayName = "Primary"),
	EAII_Secondary = 6 UMETA(DisplayName = "Secondary"),
	EAII_Interact = 7 UMETA(DisplayName = "Interact"),
	EAII_Jump = 8 UMETA(DisplayName = "Jump")
};

/**
 * Ability base that supports Icons, Ability Charges through Effect Stacking (Assumes Full Replication mode on Asc), 
 * Spawning Predictable Actors such as projectiles.
 */
UCLASS(Abstract, BlueprintType, Blueprintable, DisplayName="Ability Base")
class BONESPLIT_API UBSAbilityBase : public UCharacterAbilityBase, public IIconThumbnailInterface
{
	GENERATED_BODY()
	
public:
	
	UBSAbilityBase();
	
	virtual float GetCostModifier() const override;
	
	virtual float GetCooldownModifier() const override;
	
	virtual void OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
	virtual TSoftObjectPtr<UTexture2D> GetIcon_Implementation() const override;
	
	void Native_OnGameplayEventReceived(FGameplayTag EventTag, const FGameplayEventData* Payload);
	
	UFUNCTION(BlueprintNativeEvent, DisplayName="OnGameplayEvent")
	void BP_OnGameplayEventReceived(FGameplayTag EventTag, const FGameplayEventData Payload);
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TSoftObjectPtr<UTexture2D> AbilityIcon;
	
	virtual void EndAbility(
		const FGameplayAbilitySpecHandle Handle, 
		const FGameplayAbilityActorInfo* ActorInfo, 
		const FGameplayAbilityActivationInfo ActivationInfo, 
		bool bReplicateEndAbility, 
		bool bWasCancelled) override;
	
	UFUNCTION(BlueprintCallable)
	virtual void SpawnProjectileForPlayer(TSubclassOf<ABSProjectileBase> Projectile, FTransform SpawnTransform);
	
	UFUNCTION(BlueprintCallable)
	virtual void SpawnProjectileForMob(TSubclassOf<ABSProjectileBase> Projectile, FTransform SpawnTransform, int32 NumProjectiles = 1, float ConeAngle = 15, bool bScaleWithMultiHit = true);
	
	UFUNCTION(BlueprintCallable)
	void GetTargetRotationForProjectile(const FVector& ProjectileOrigin, FVector& BaseAimDirection, FVector& OutProjectileBaseAimPoint, FVector& OutCameraBaseAimPoint) const;
	
protected:
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Player")
	TEnumAsByte<EBSAbilityInputID> InputID = EAII_None;
	
	UFUNCTION(BlueprintPure)
	UBSAbilitySystemComponent* GetBSAbilitySystemComponent() const;
	
	//Event tags to listen to. OnGameplayEvent will fire if any event was received that matches these tags.
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta=(Categories="AnimEvent"))
	FGameplayTagContainer EventTagsToListenTo;
	
	FDelegateHandle EventHandle;
};
