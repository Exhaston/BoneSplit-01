// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonButtonBase.h"
#include "BSWButtonBase.generated.h"

class UCommonTextBlock;
/**
 * 
 */
UCLASS()
class BONESPLIT_API UBSWButtonBase : public UCommonButtonBase
{
	GENERATED_BODY()
	
public:
	
	virtual void NativePreConstruct() override;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FText ButtonText;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta=(BindWidget))
	UCommonTextBlock* ButtonTextBlock;
};
