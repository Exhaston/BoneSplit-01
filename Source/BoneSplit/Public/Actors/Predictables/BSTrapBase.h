// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Actor.h"
#include "BSTrapBase.generated.h"


class UAbilitySystemComponent;
class UGameplayEffect;

UCLASS(DisplayName="Trap Base", Abstract, BlueprintType, Blueprintable)
class BONESPLIT_API ABSTrapBase : public AActor, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	
	ABSTrapBase();

protected:
	
	virtual void BeginPlay() override;
	
	virtual void PostInitializeComponents() override;

public:
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	virtual void Tick(float DeltaTime) override;
	
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	
	UFUNCTION(NetMulticast, Reliable)
	void PlayAnim();
	
	UPROPERTY()
	FTimeline AnimTimeLine;
	
	FOnTimelineFloat AnimProgressDelegate;
	
	FOnTimelineEvent OnTimelineFinishedDelegate;
	
	UFUNCTION()
	void HandleAnimProgress(float Value);
	
	UFUNCTION()
	void OnTimeLineFinished();
	
	void TriggerTrap();
	
	void ResetTrap();
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> EffectToApply;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FVector OriginDirection = {0, 0, -1};
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FName MaterialParamName = FName("Time");
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FRuntimeFloatCurve AnimationCurve;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	UStaticMeshComponent* TrapMesh;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta=(Units=seconds))
	float AnimationSpeed = 0.5f;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta=(Units=seconds))
	float Cooldown = 60.f;
	
	UPROPERTY()
	bool bReady = true;
	
	UPROPERTY()
	UAbilitySystemComponent* AbilitySystemComponent;
};
