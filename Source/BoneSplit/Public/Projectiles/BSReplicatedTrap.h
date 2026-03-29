// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "NativeGameplayTags.h"
#include "Components/TimelineComponent.h"
#include "Factions/FactionInterface.h"
#include "GameFramework/Actor.h"
#include "BSReplicatedTrap.generated.h"

class UCharacterAbilitySystem;
class UNiagaraComponent;
class UAbilitySystemComponent;

namespace BSGameplayTags
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Faction_Trap);
}

UCLASS(Abstract, BlueprintType, Blueprintable)
class BONESPLIT_API ABSReplicatedTrap : public AActor, public IFactionInterface, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	
	explicit ABSReplicatedTrap(const FObjectInitializer& ObjectInitializer);
	
	virtual FGameplayTagContainer& GetFactionTags() override;
	
	virtual bool HasAnyMatchingFactionTag(FGameplayTagContainer InFactionTags) override;
	
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	UPROPERTY(EditDefaultsOnly, Category="Throw")
	UCurveFloat* ThrowCurve;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Throw", Replicated, meta=(ExposeOnSpawn))
	float ArcHeight = 200.f;
	
	virtual void Tick(float DeltaSeconds) override;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:       
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	bool bHasLanded = false;
	
	FTimeline ThrowTimeline;
	                                    
	virtual void BeginPlay() override;     
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category="BoneSplit|Trap")
	bool bSnapToTarget = true;
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category="BoneSplit|Trap")
	bool bParentToTarget = true;
	
	//Owning actors can append their faction tags to avoid trigger
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category="BoneSplit|Trap", meta=(ExposeOnSpawn), Replicated)
	FGameplayTagContainer FactionTags = FGameplayTagContainer(BSGameplayTags::Faction_Trap);
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category="BoneSplit|Trap")
	USkeletalMeshComponent* SkeletalMeshComponent = nullptr;
	
	FVector StartLocation;
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category="BoneSplit|Trap", meta=(ExposeOnSpawn), Replicated)
	FVector EndLocation;
	

	
	// Bound to the timeline's update
	UFUNCTION()
	void OnThrowUpdate(float Alpha);
	
	UFUNCTION(BlueprintNativeEvent, DisplayName="OnThrowTick")
	void BP_OnThrowTick(float Alpha);

	// Bound to the timeline's finish
	UFUNCTION()
	void OnThrowFinished();
	
	UFUNCTION(BlueprintNativeEvent)
	void OnLanded();
	
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category="BoneSplit|Trap")
	TObjectPtr<UCharacterAbilitySystem> Asc;
};
