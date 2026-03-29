// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "IconThumbnailInterface.h"
#include "Engine/DataAsset.h"
#include "BSPowerUpData.generated.h"

class UCharacterAbilityBase;
class UBSPowerUpEffect;

UENUM(BlueprintType)
enum EBSPowerUpRarity
{
	EPR_Common UMETA(DisplayName="Common"),
	EPR_Rare UMETA(DisplayName="Rare"),
	EPR_Epic UMETA(DisplayName="Epic"),                               
	EPR_Legendary UMETA(DisplayName="Legendary"),
};

/**
 * 
 */
UCLASS(Const, Blueprintable, BlueprintType)
class BONESPLIT_API UBSPowerUpData : public UPrimaryDataAsset, public IIconThumbnailInterface
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UTexture2D> PowerUpIcon;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor PowerUpIconTint = FLinearColor::White;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EBSPowerUpRarity> PowerUpRarity = EPR_Common;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText PowerUpName;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(MultiLine=true))
	FText PowerUpInfo;
	
	virtual TSoftObjectPtr<UTexture2D> GetIcon_Implementation() const override { return PowerUpIcon; }
	virtual FLinearColor GetTint_Implementation() const override { return PowerUpIconTint; }
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTagContainer GrantedTags;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSubclassOf<UCharacterAbilityBase>> GrantedAbilities;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSubclassOf<UBSPowerUpEffect>> GrantedEffects;
	
};
