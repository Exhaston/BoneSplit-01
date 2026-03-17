// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "GameplayAbilitySpecHandle.h"
#include "BSActionButton.generated.h"

class UAbilityBufferComponent;
class UCharacterAbilitySystem;
class UBSAbilitySystemComponent;
class UCommonRichTextBlock;
class UGameplayAbility;
struct FGameplayAbilitySpec;
class UAbilitySystemComponent;
class UCommonLazyImage;

extern TAutoConsoleVariable<bool> CVarToggleCooldownNumbers;

//TODO: NEEDS REWORK. INEFFICIENT LOOPING DURING TICK.
/**
 * 
 */
UCLASS()
class BONESPLIT_API UBSActionButton : public UCommonUserWidget
{
	GENERATED_BODY()
	
public:
	
	virtual void InitializeActionButton(UCharacterAbilitySystem* InAbilitySystemComponent);
	
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
