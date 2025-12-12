// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "BSFiniteState.generated.h"

class UAbilitySystemComponent;
class UBSFiniteStateComponent;
/**
 * 
 */
UCLASS(Abstract, BlueprintType, Blueprintable, DisplayName="Finite State Base")
class BONESPLIT_API UBSFiniteState : public UObject
{
	GENERATED_BODY()
	
public:
	
	virtual void NativeEnterState(UBSFiniteStateComponent* InOwnerComponent);
	virtual void NativeTickState(UBSFiniteStateComponent* InOwnerComponent, float DeltaTime);
	virtual void NativeEndState(UBSFiniteStateComponent* InOwnerComponent);
	
	virtual UWorld* GetWorld() const override;
	
	//Runs when the state is initially created. This is guaranteed to run before the first tick.
	UFUNCTION(BlueprintNativeEvent, Category="FiniteState", DisplayName="On Enter State")
	void BP_OnEnterState(UBSFiniteStateComponent* InOwnerComponent);
	
	UFUNCTION(BlueprintNativeEvent, Category="FiniteState", DisplayName="On Tick State")
	void BP_OnTickState(UBSFiniteStateComponent* InOwnerComponent, float DeltaTime);
	
	UFUNCTION(BlueprintNativeEvent, Category="FiniteState", DisplayName="On End State")
	void BP_OnEndState(UBSFiniteStateComponent* InOwnerComponent);
	
	UFUNCTION(BlueprintCallable, Category="FiniteState", DisplayName="Change State")
	void ChangeState(TSubclassOf<UBSFiniteState> NewState) const;
	
	UFUNCTION(BlueprintPure, Category="FiniteState")
	UBSFiniteStateComponent* GetOwnerComponent() const;
	
	UFUNCTION(BlueprintPure, Category="FiniteState")
	AActor* GetOwnerActor() const;
	
	//Attempts to get the ability system from the owner actor. 
	//Can return null if owner doesn't have an ability system, or it isn't initialized or replicated yet.
	UFUNCTION(BlueprintPure, Category="FiniteState")
	UAbilitySystemComponent* TryGetAbilitySystemComponent() const;
	
	virtual bool GetShouldPool() const;
	
protected:
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	bool bPooled = true;
	
	UPROPERTY()
	bool bCanTick = false;
	
	UPROPERTY()
	UBSFiniteStateComponent* OwnerComponent;
};
