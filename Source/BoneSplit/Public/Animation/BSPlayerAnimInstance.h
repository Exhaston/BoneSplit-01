// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BSAnimInstance.h"
#include "Components/Inventory/BSEquipment.h"
#include "BSPlayerAnimInstance.generated.h"

class UBSEquipmentEffect;
class ABSPlayerCharacter;

/**
 * 
 */
UCLASS(DisplayName="BSPlayerAnimInstance")
class BONESPLIT_API UBSPlayerAnimInstance : public UBSAnimInstance
{
	GENERATED_BODY()
	
public:
	
	virtual void InitializeAbilitySystemComponent(UAbilitySystemComponent* InAbilitySystemComponent) override;
	
	virtual void PreUpdateAnimation(float DeltaSeconds) override;
	
	virtual void OnAnyTagChanged(const FGameplayTag Tag, int32 Count) override;
	
	UFUNCTION(BlueprintNativeEvent, Category="BoneSplit", DisplayName="OnEquipmentChanged")
	void BP_OnEquipmentChanged();
	
	UFUNCTION(BlueprintNativeEvent, Category="BoneSplit", DisplayName="OnWeaponTagChanged")
	void BP_OnWeaponTagChanged(FGameplayTag NewWeaponTypeTag, FGameplayTag OldWeaponTypeTag);
	
	//Always validate before use. Could invalidate under destruction etc.
	UFUNCTION(BlueprintPure, Category="Bone Split")
	ABSPlayerCharacter* GetPlayerCharacter() const { return PlayerCharacterOwner; }
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	USkeletalMesh* HeadMeshAsset = nullptr;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	USkeletalMesh* ChestMeshAsset = nullptr;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	USkeletalMesh* ArmsMeshAsset = nullptr;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	USkeletalMesh* LegsMeshAsset = nullptr;
	
protected:
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta=(Categories="BoneSplit|Weapon"))
	FGameplayTag WeaponTypeTag = BSTags::WeaponType_SwordNBoard;
	
	UPROPERTY(BlueprintReadOnly, Category="BoneSplit")
	ABSPlayerCharacter* PlayerCharacterOwner;
	
};
