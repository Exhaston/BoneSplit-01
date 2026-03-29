// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AdvancedProgressBarWidget.generated.h"

class UImage;
/**
 * 
 */
UCLASS()
class ADVANCEDPROGRESSBAR_API UAdvancedProgressBarWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	
	virtual void NativePreConstruct() override;
	
	virtual void SetProgress(float NewProgress);
	virtual void SetBonusProgress(float NewBonusProgress);
	
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float DeltaInterpSpeed = 8;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FLinearColor FillColor = FLinearColor::Red;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FLinearColor DeltaColor = FLinearColor::White;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FLinearColor BonusColor = FLinearColor::Green;
	
	float BonusProgress = 0;
	
	float Progress = 0;
	float OldProgress = 0;
	
protected:
	
	UMaterialInstanceDynamic* GetBarMaterial();
	
	UPROPERTY(meta=(BindWidget))
	UImage* ProgressBarImage;
	
};
