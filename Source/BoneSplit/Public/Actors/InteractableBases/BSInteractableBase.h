// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BSLockableActor.h"
#include "GameFramework/Actor.h"
#include "Components/InteractionSystem/BSInteractableInterface.h"
#include "BSInteractableBase.generated.h"                                                          

class UWidgetComponent;

UCLASS(DisplayName="Interactable Base", Category="BoneSplit")
class BONESPLIT_API ABSInteractableBase : public ABSLockableActor, public IBSInteractableInterface
{
	GENERATED_BODY()

public:
	
	explicit ABSInteractableBase(const FObjectInitializer& ObjectInitializer);
	
	virtual bool TryInteract(UBSInteractionComponent* InteractInstigator) override;
	
	UFUNCTION(BlueprintNativeEvent)
	bool BP_TryInteract(UBSInteractionComponent* InteractInstigator);
	
	virtual void FocusInteractable(UBSInteractionComponent* InteractInstigator) override;
	
	UFUNCTION(BlueprintNativeEvent)
	void BP_OnFocusedForInteraction(UBSInteractionComponent* InteractInstigator);
	
	virtual void UnFocusInteractable(UBSInteractionComponent* InteractInstigator) override;
	
	UFUNCTION(BlueprintNativeEvent)
	void BP_OnUnFocusForInteraction(UBSInteractionComponent* InteractInstigator);
	
	virtual bool IsInteractable(UBSInteractionComponent* InteractInstigator) override;
	
	UFUNCTION(BlueprintNativeEvent)
	bool BP_IsInteractable(UBSInteractionComponent* InteractInstigator);
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	UWidgetComponent* WidgetComponent;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TSubclassOf<UUserWidget> InteractionWidgetClass;
};
