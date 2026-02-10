// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "GameplayAbilitySpecHandle.h"
#include "GameplayTagContainer.h"
#include "BoneSplit/BoneSplit.h"
#include "BSActionButton.generated.h"

class UCommonRichTextBlock;
class UGameplayAbility;
struct FGameplayAbilitySpec;
class UAbilitySystemComponent;
class UCommonLazyImage;
/**
 * 
 */
UCLASS()
class BONESPLIT_API UBSActionButton : public UCommonUserWidget
{
	GENERATED_BODY()
	
public:
	
	virtual void InitializeActionButton(UAbilitySystemComponent* InAbilitySystemComponent);
	
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
	void Test(UGameplayAbility* AbilityInstance, float& TimeRemaining, float& CooldownDuration);
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const;
	
	UMaterialInstanceDynamic* GetButtonMaterial() const;
	
	FGameplayAbilitySpec* GetAbilitySpecForID();
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta=(Categories="Ability"))
	int32 InputID;
	
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
	
protected:
	
	UPROPERTY()
	FGameplayAbilitySpecHandle CurrentCachedHandle;
	
	UPROPERTY(Transient)
	TWeakObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
};
