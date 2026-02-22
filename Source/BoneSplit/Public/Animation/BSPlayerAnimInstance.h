// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BSAnimInstance.h"
#include "BSPlayerAnimInstance.generated.h"

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
	
	UFUNCTION(BlueprintNativeEvent, Category="BoneSplit", DisplayName="OnEquipmentMeshChanged")
	void BP_OnEquipmentMeshChanged(FGameplayTag MeshTag, USkeletalMesh* NewSkeletalMesh);
	
	UFUNCTION(BlueprintNativeEvent, Category="BoneSplit", DisplayName="OnWeaponTagChanged")
	void BP_OnWeaponTagChanged(FGameplayTag NewWeaponTypeTag, FGameplayTag OldWeaponTypeTag);
	
	UPROPERTY(BlueprintReadOnly, Category="Bone Split")
	USkeletalMesh* HeadMeshAsset;
	UPROPERTY(BlueprintReadOnly, Category="Bone Split")
	USkeletalMesh* ChestMeshAsset;
	UPROPERTY(BlueprintReadOnly, Category="Bone Split")
	USkeletalMesh* LegsMeshAsset;
	UPROPERTY(BlueprintReadOnly, Category="Bone Split")
	USkeletalMesh* ArmsMeshAsset;
	
	//Always validate before use. Could invalidate under destruction etc.
	UFUNCTION(BlueprintPure, Category="Bone Split")
	ABSPlayerCharacter* GetPlayerCharacter() const { return PlayerCharacterOwner; }
	
protected:
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta=(Categories="BoneSplit|Weapon"))
	FGameplayTag WeaponTypeTag = BSTags::WeaponType_SwordNBoard;
	
	UPROPERTY(BlueprintReadOnly, Category="BoneSplit")
	ABSPlayerCharacter* PlayerCharacterOwner;
	
};
