// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CharacterAbilityBase.h"
#include "IconThumbnailInterface.h"
#include "BSPlayerAbilityBase.generated.h"

UENUM(BlueprintType, DisplayName="Ability Input ID")
enum EBSAbilityInputID                
{   
	EAII_None = 0 UMETA(DisplayName = "None"),
	EAII_Special = 1 UMETA(DisplayName = "Special"),                 
	EAII_Head = 2 UMETA(DisplayName = "Head"),
	EAII_Legs = 3 UMETA(DisplayName = "Legs"),
	EAII_Soul = 4 UMETA(DisplayName = "Soul"),
	EAII_Primary = 5 UMETA(DisplayName = "Primary"),
	EAII_Secondary = 6 UMETA(DisplayName = "Secondary"),
	EAII_Interact = 7 UMETA(DisplayName = "Interact"),
	EAII_Jump = 8 UMETA(DisplayName = "Jump")
};

/**                                      
 * 
 */
UCLASS(Blueprintable, BlueprintType, Abstract, DisplayName="Player Ability Base")
class BONESPLIT_API UBSPlayerAbilityBase : public UCharacterAbilityBase, public IIconThumbnailInterface
{
	GENERATED_BODY()
	
public:
	
	virtual TSoftObjectPtr<UTexture2D> GetIcon_Implementation() const override;
	
	virtual FLinearColor GetTint_Implementation() const override;
	
	virtual void OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	
	virtual float GetCostModifier() const override;
	
	virtual float GetCooldownModifier() const override;
	
protected:
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TEnumAsByte<EBSAbilityInputID> InputID = EAII_None;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TSoftObjectPtr<UTexture2D> AbilityIcon;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FLinearColor IconTint = FLinearColor::White;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FText AbilityName;
};
