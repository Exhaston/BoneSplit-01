// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BSPowerUpComponent.h"
#include "CommonUserWidget.h"
#include "BSWidget_PowerUpSelector.generated.h"

class UBSWidget_PowerChoiceButton;
/**
 * 
 */
UCLASS(Abstract, BlueprintType, Blueprintable)
class BONESPLIT_API UBSWidget_PowerUpSelector : public UCommonUserWidget
{
	GENERATED_BODY()
	
public:

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	void InitializeSelector(const FBSAvailableChoice& InChoice);

	DECLARE_MULTICAST_DELEGATE_TwoParams(FOnPowerUpSelected, FBSAvailableChoice, int32 /*Index*/);
	FOnPowerUpSelected OnPowerUpSelected;

protected:

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UPanelWidget> PowerButtonParent;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UBSWidget_PowerChoiceButton> ButtonClass;

private:

	void HandleButtonClicked(int32 Index);

	UPROPERTY()
	FBSAvailableChoice CurrentChoice;

	UPROPERTY()
	TArray<TObjectPtr<UBSWidget_PowerChoiceButton>> SpawnedButtons;
};
