// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilitySpecHandle.h"
#include "Blueprint/UserWidget.h"
#include "BSWidget_ActionButton.generated.h"

struct FGameplayAbilitySpec;
class UGameplayAbility;
class UAbilityBufferComponent;
class UCharacterAbilitySystem;
class UCommonRichTextBlock;
class UCommonLazyImage;

namespace BSConsoleVariables
{
	extern TAutoConsoleVariable<bool> CVarToggleCooldownNumbers;
}


/**
 * 
 */
UCLASS()
class BONESPLIT_API UBSWidget_ActionButton : public UUserWidget
{
	GENERATED_BODY()
	
public:
	
	virtual void NativeConstruct() override;
	
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
	UMaterialInstanceDynamic* GetButtonMaterial() const;

protected:
	
	virtual bool AttemptSetNewSpec(FGameplayAbilitySpec* Spec);
	
	virtual void DisplayFallbackIcon();
	
	virtual void ResetActionButton();
	
	virtual void TrySetAbilityIcon(const UGameplayAbility* AbilityInstance);
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta=(Categories="Ability"))
	int32 InputID = -1;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FName TextureParamName = FName("IconTexture");
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FName PercentParamName = FName("Percent");
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FName CanActivateParam = FName("CanActivate");
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FName PressedParamName = FName("Pressed");
	
	UPROPERTY(meta=(BindWidget))
	UCommonLazyImage* AbilityIcon;
	
	UPROPERTY(meta=(BindWidget))
	UCommonRichTextBlock* CooldownNumberText;
	
	UPROPERTY(meta=(BindWidget))
	UCommonRichTextBlock* AbilityChargeText;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	UTexture2D* FallbackIcon;
	
	UPROPERTY(Transient)
	TObjectPtr<UCharacterAbilitySystem> AbilitySystemComponent;
	
	UPROPERTY()
	FGameplayAbilitySpecHandle CurrentCachedHandle;
	
	UPROPERTY()
	float BufferTime = 0.5f;
	
	UPROPERTY()
	float CurrentBufferElapsed = 0;
	
	UPROPERTY(Transient)
	TObjectPtr<UAbilityBufferComponent> AbilityBufferComponent;
};
