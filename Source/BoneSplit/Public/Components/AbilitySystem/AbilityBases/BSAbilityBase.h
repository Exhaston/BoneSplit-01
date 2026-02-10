// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "IconThumbnailInterface.h"
#include "Abilities/GameplayAbility.h"
#include "BoneSplit/BoneSplit.h"
#include "BSAbilityBase.generated.h"

class ABSProjectileBase;
class UBSAbilitySystemComponent;
enum EBSAbilityInputID : uint8;
class ABSPredictableActor;

#define COMMIT_ABILITY(Handle, ActorInfo, ActivationInfo)   \
if (!CommitAbility(Handle, ActorInfo, ActivationInfo))      \
{                                                           \
	CancelAbility(Handle, ActorInfo, ActivationInfo, true); \
	return;                                                 \
}
#define CANCEL_ABILITY() \
CancelAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true); return;
/**
 * Ability base that supports Icons, Ability Charges through Effect Stacking (Assumes Full Replication mode on Asc), 
 * Spawning Predictable Actors such as projectiles.
 */
UCLASS(Abstract, BlueprintType, Blueprintable, DisplayName="Ability Base")
class BONESPLIT_API UBSAbilityBase : public UGameplayAbility, public IIconThumbnailInterface
{
	GENERATED_BODY()
	
public:
	
	UBSAbilityBase();
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
	virtual TSoftObjectPtr<UTexture2D> GetIcon_Implementation() const override;
	
	virtual bool CheckCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	
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
	void GetTargetRotationForProjectile(const FVector& ProjectileOrigin, FVector& BaseAimDirection, FVector& OutProjectileBaseAimPoint, FVector& OutCameraBaseAimPoint) const;
	
protected:
	
	UFUNCTION(BlueprintPure)
	UBSAbilitySystemComponent* GetBSAbilitySystemComponent() const;
	
	//Event tags to listen to. OnGameplayEvent will fire if any event was received that matches these tags.
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta=(Categories="AnimEvent"))
	FGameplayTagContainer EventTagsToListenTo;
	
	FDelegateHandle EventHandle;
};
