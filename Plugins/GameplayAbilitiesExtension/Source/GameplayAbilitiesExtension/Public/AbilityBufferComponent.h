// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CharacterAbilityBase.h"
#include "GameplayTagContainer.h"
#include "InputAction.h"
#include "Components/ActorComponent.h"
#include "AbilityBufferComponent.generated.h"

class UCharacterAbilitySystem;

USTRUCT(BlueprintType)
struct FAbilityInputBindData
{
	GENERATED_BODY()
	
	FAbilityInputBindData() = default;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	UInputAction* InputAction = nullptr;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	int32 AbilityInputID = -1;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	bool bRetriggerEveryFrame = false;
	
	bool operator==(const FAbilityInputBindData& Other) const
	{
		return AbilityInputID == Other.AbilityInputID || InputAction == Other.InputAction;
	}
};

USTRUCT(BlueprintType)
struct FAbilityBufferData
{
	GENERATED_BODY()
	
	FAbilityBufferData() = default;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	int32 AbilityInputID = -1;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TEnumAsByte<EAbilityBufferMode> BufferChannel = EABT_Unique;
	
	UPROPERTY()
	float TimeRemaining = 0;
	
	bool operator==(const FAbilityBufferData& Other) const
	{
		return AbilityInputID == Other.AbilityInputID;
	}
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GAMEPLAYABILITIESEXTENSION_API UAbilityBufferComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	
	UAbilityBufferComponent();
	
	virtual void BindAbilityInputActions(UEnhancedInputComponent* EnhancedInputComponent);

	static UAbilityBufferComponent* GetBufferComponentFromController(const AController* PC);
	virtual void SetAbilitySystemComponent(UCharacterAbilitySystem* InAsc);
	
	UFUNCTION(BlueprintPure, Category="AbilityExtension")
	bool GetIsBufferedOrPressed(int32 InputID) const;
	
	UFUNCTION(BlueprintCallable, Category = "AbilityExtension")
	bool GetIsAbilityBuffered(int32 InputID) const;
	
	UFUNCTION(BlueprintCallable, Category = "AbilityExtension")
	bool TryActivateAbility(int32 InputID) const;
	
	virtual void NotifyInputPressed(int32 InputID);
	virtual void NotifyInputReleased(int32 InputID);
	
	UFUNCTION(BlueprintCallable, Category = "AbilityExtension")
	void TryBufferAbilityByID(int32 InputID);
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

protected:
	
	void BufferAbility(TEnumAsByte<EAbilityBufferMode> BufferMode, int32 AbilityID);
	
	//How long an ability can be queried for activation every tick after attempted activation.
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta=(ClampMin=0, Units=seconds))
	float BufferTime = 0.5f;
	
	UPROPERTY()
	TArray<int32> PressedInputs;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TArray<FAbilityInputBindData> AbilityInputActions;
	
	UPROPERTY(Transient)
	TArray<FAbilityBufferData> BufferedAbilities;
	
	UPROPERTY(Transient)
	TObjectPtr<UCharacterAbilitySystem> CharacterAbilitySystem;
};
