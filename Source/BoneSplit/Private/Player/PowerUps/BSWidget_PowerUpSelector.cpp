// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Player/PowerUps/BSWidget_PowerUpSelector.h"

#include "Components/PanelWidget.h"
#include "GameFramework/PlayerState.h"
#include "Player/PowerUps/BSWidget_PowerChoiceButton.h"

void UBSWidget_PowerUpSelector::NativeConstruct()
{
	Super::NativeConstruct();
}

void UBSWidget_PowerUpSelector::NativeDestruct()
{
	// Clear button callbacks before destruction
	for (UBSWidget_PowerChoiceButton* Button : SpawnedButtons)
	{
		if (Button)
		{
			Button->OnClicked().RemoveAll(this);
		}
	}

	Super::NativeDestruct();
}

void UBSWidget_PowerUpSelector::InitializeSelector(const FBSAvailableChoice& InChoice)
{
	if (!ButtonClass || !PowerButtonParent) return;

	CurrentChoice = InChoice;
	PowerButtonParent->ClearChildren();
	SpawnedButtons.Reset();

	for (int32 i = 0; i < InChoice.ChoiceData.Num(); i++)
	{
		UBSPowerUpData* Data = InChoice.ChoiceData[i];
		if (!Data) continue;

		UBSWidget_PowerChoiceButton* Button = CreateWidget<UBSWidget_PowerChoiceButton>(this, ButtonClass);
		if (!Button) continue;

		Button->InitializeButton(Data);

		// Capture index by value for the lambda
		Button->OnClicked().AddLambda([this, i]()
		{
			HandleButtonClicked(i);
		});

		PowerButtonParent->AddChild(Button);
		SpawnedButtons.Add(Button);
	}
	
	APlayerController* PC = GetOwningPlayer();
	if (!PC) return;

	PC->SetShowMouseCursor(true);
	PC->ResetIgnoreMoveInput(); // ensure movement input is still processed

	FInputModeGameAndUI InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputMode.SetHideCursorDuringCapture(false);
	PC->SetInputMode(InputMode);
}

void UBSWidget_PowerUpSelector::HandleButtonClicked(const int32 Index)
{
	if (UBSPowerUpComponent* PowerUpComponent = UBSPowerUpComponent::GetPowerUpComponent(GetOwningPlayerState()))
	{
		PowerUpComponent->ClaimPowerUp(CurrentChoice, Index);
		APlayerController* PC = GetOwningPlayer();
		if (!PC) return;
		PC->SetShowMouseCursor(false);
		PC->SetInputMode(FInputModeGameOnly());
		RemoveFromParent();
	}
	OnPowerUpSelected.Broadcast(CurrentChoice, Index);
}
