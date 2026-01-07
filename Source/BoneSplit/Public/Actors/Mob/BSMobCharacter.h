// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "Interfaces/BSMovementInterface.h"
#include "BSMobCharacter.generated.h"

class UBSAttributeSet;
class UGameplayAbility;
class UGameplayEffect;
class UBSAbilitySystemComponent;
class UBSMobMovementComponent;


UCLASS()
class BONESPLIT_API ABSMobCharacter : public ACharacter, public IAbilitySystemInterface, public IBSMovementInterface
{
	GENERATED_BODY()

public:
	
	explicit ABSMobCharacter(const FObjectInitializer& ObjectInitializer);
	
	virtual void BeginPlay() override;
	
	virtual void LaunchActor(FVector Direction, float Magnitude) override;
	
	virtual void SetMovementRotationMode(uint8 NewMovementMode) override;
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

protected:
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TArray<TSubclassOf<UGameplayEffect>> Effects;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TArray<TSubclassOf<UGameplayAbility>> GameplayAbility;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TObjectPtr<UBSAttributeSet> AttributeSet;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	UBSAbilitySystemComponent* AbilitySystemComponent;
};
