// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "GameplayAbilitySpecHandle.h"
#include "GameplayTagContainer.h"
#include "BoneSplit/BoneSplit.h"
#include "BSActionButton.generated.h"

class UBSAbilitySystemComponent;
class UCommonRichTextBlock;
class UGameplayAbility;
struct FGameplayAbilitySpec;
class UAbilitySystemComponent;
class UCommonLazyImage;

//TODO: NEEDS REWORK. INEFFICIENT LOOPING DURING TICK.
/**
 * 
 */
UCLASS()
class BONESPLIT_API UBSActionButton : public UCommonUserWidget
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
	FGameplayTag AbilityTag;
	
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
	UCommonRichTextBlock* AbilityChargeText;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	UTexture2D* FallbackIcon;
	
	UPROPERTY(Transient)
	TWeakObjectPtr<UBSAbilitySystemComponent> AbilitySystemComponent;
	
	UPROPERTY()
	FGameplayAbilitySpecHandle CurrentCachedHandle;
};
