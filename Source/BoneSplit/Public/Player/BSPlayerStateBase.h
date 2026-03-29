// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagAssetInterface.h"
#include "GameFramework/PlayerState.h"
#include "BSPlayerStateBase.generated.h"

class UBSPowerUpComponent;
class UBSExtendedAttributeSet;
class UBSAbilitySystem;

/**
 *                                          
 */
UCLASS(Abstract, Blueprintable, BlueprintType, DisplayName="BS Player State Base")
class BONESPLIT_API ABSPlayerStateBase : public APlayerState, public IAbilitySystemInterface, 
public IGameplayTagAssetInterface
{
	GENERATED_BODY()
	
public:
	
	explicit ABSPlayerStateBase(const FObjectInitializer& ObjectInitializer);
	
	virtual void ClientInitialize(AController* C) override;
	
	virtual void CopyProperties(APlayerState* PlayerState) override;
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;
	virtual bool HasMatchingGameplayTag(FGameplayTag TagToCheck) const override;
	virtual bool HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;
	virtual bool HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;
	
	virtual void SetHasAbilitySystemData()
	{
		if (HasAuthority()) 
			bHasAbilitySystemData = true;
	}
	
	virtual bool GetHasAbilitySystemData()
	{
		return bHasAbilitySystemData;
	}
	
	virtual UBSExtendedAttributeSet* GetAttributeSet();
	
	virtual UBSPowerUpComponent* GetPowerUpComponent() { return PowerUpComponent; }
	
protected:
	
	bool bHasAbilitySystemData = false;
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<UBSAbilitySystem> AbilitySystemComponent;
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<UBSExtendedAttributeSet> AttributeSet;
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<UBSPowerUpComponent> PowerUpComponent;
};
