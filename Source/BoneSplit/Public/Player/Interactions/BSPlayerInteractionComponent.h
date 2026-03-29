// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BSPlayerInteractionComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class BONESPLIT_API UBSPlayerInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UBSPlayerInteractionComponent();
	
	static UBSPlayerInteractionComponent* GetInteractionComponent(const AActor* InTarget);

	// Call this from your input binding (Character or PlayerController)
	UFUNCTION(BlueprintCallable, Category="Interaction")
	void TryInteract() const;

	UFUNCTION(BlueprintPure, Category="Interaction")
	AActor* GetFocusedActor() const { return FocusedActor; }

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
							   FActorComponentTickFunction* ThisTickFunction) override;

private:
	// Radius of the sphere overlap used to detect interactables
	UPROPERTY(EditAnywhere, Category="Interaction", meta=(ClampMin="0.0"))
	float InteractionRadius = 200.f;

	// Collision channel your interactable actors respond to
	UPROPERTY(EditAnywhere, Category="Interaction")
	TEnumAsByte<ECollisionChannel> InteractionChannel = ECC_WorldDynamic;

	UPROPERTY(Transient)
	TObjectPtr<AActor> FocusedActor = nullptr;

	void UpdateFocus();
	AActor* FindClosestInteractable() const;
};
