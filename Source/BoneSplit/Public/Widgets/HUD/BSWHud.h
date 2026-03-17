// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "CommonActivatableWidget.h"
#include "BSWHud.generated.h"

class UBSBuffIcon;
class UBSBuffEffect;
class UCharacterAbilitySystem;
class UCommonLazyImage;
class UBSAbilitySystemComponent;
class UBSActionButton;
class UBSAttributeBar;
/**
 * 
 */
UCLASS(Abstract, BlueprintType, Blueprintable)
class BONESPLIT_API UBSWHud : public UCommonActivatableWidget
{
	GENERATED_BODY()
	
public:
	
	virtual void InitializePlayerHUD(UCharacterAbilitySystem* InAbilitySystemComponent);
	
	virtual void NativePreConstruct() override;
	
	virtual void NativeConstruct() override;
	
	virtual void NativeOnActivated() override;
	
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

protected:
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TSubclassOf<UBSBuffIcon> BuffIconClass;
	
	UPROPERTY()
	TMap<const UBSBuffEffect*, UBSBuffIcon*> BuffMap;
	
	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UCommonLazyImage> HookCrosshairs;
	
	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UBSAttributeBar> HealthBar;
	
	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UBSAttributeBar> ManaBar;
	
	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UBSAttributeBar> SoulBar;
	
	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UBSActionButton> ActionButton1;
	
	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UBSActionButton> ActionButton2;
	
	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UBSActionButton> ActionButton3;
	
	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UBSActionButton> ActionButton4;
	
	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UBSActionButton> ActionButton5;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UPanelWidget> BuffParentSlot;
	
	UPROPERTY(Transient)
	TWeakObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
	
};
