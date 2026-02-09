// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BSInteractionComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBSOnFocusChanged, AActor*, NewInteractionFocus);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class BONESPLIT_API UBSInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	
	UBSInteractionComponent();
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
	
	bool TryInteract();
	
	AActor* GetCurrentInteractable() const;
	
	FBSOnFocusChanged& GetOnFocusChangedDelegate() { return OnFocusChanged; }
	
protected:
	
	//Will broadcast null if no interactions are around.
	UPROPERTY(BlueprintAssignable)
	FBSOnFocusChanged OnFocusChanged;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta=(ClampMin=0, Units=Centimeters))
	float InteractionRadius = 150;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TEnumAsByte<ECollisionChannel> InteractionChannel = ECC_GameTraceChannel18;
	
	UPROPERTY()
	TWeakObjectPtr<AActor> CurrentFocus;
};
