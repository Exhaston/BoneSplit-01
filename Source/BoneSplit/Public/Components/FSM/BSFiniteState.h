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
	
	virtual UWorld* GetWorld() const override;
	
	virtual void NativeEnterState(UBSFiniteStateComponent* InOwnerComponent);
	
	virtual void NativeTickState(UBSFiniteStateComponent* InOwnerComponent, float DeltaTime);
	
	virtual void NativeEndState(UBSFiniteStateComponent* InOwnerComponent);
	
	//Runs when the state is initially created or re-entered through pooling. 
	//This is guaranteed to run before the first tick.
	UFUNCTION(BlueprintNativeEvent, Category="FiniteState", DisplayName="On Enter State")
	void BP_OnEnterState(UBSFiniteStateComponent* InOwnerComponent);
	
	//Will not run before On Enter State Event. After that it follows owner component's tick
	UFUNCTION(BlueprintNativeEvent, Category="FiniteState", DisplayName="On Tick State")
	void BP_OnTickState(UBSFiniteStateComponent* InOwnerComponent, float DeltaTime);
	
	//Runs before the state is either saved in a pool or marked as garbage. 
	//Stop latent actions or clean up delegate bindings here.
	UFUNCTION(BlueprintNativeEvent, Category="FiniteState", DisplayName="On End State")
	void BP_OnEndState(UBSFiniteStateComponent* InOwnerComponent);
	
	//Changes the state of the owner component. Nullptr is valid to stop state. 
	//This will trigger On End State on this state. Before disposing of it.
	UFUNCTION(BlueprintCallable, Category="FiniteState", DisplayName="Change State")
	void ChangeState(TSubclassOf<UBSFiniteState> NewState) const;
	
	UFUNCTION(BlueprintPure, Category="FiniteState")
	UBSFiniteStateComponent* GetOwnerComponent() const;
	
	UFUNCTION(BlueprintPure, Category="FiniteState")
	AActor* GetOwnerActor() const;
	
	virtual bool GetShouldPool() const;
	
protected:
	
	//Allows the state to be pooled and re-used every time it is entered. This improves performance and saves state.
	//Remember to clean up or reset variables if needed on activations.
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	bool bPooled = true;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta=(ClampMin=0, Units=Seconds))
	float TickInterval = 1;
	
	UPROPERTY()
	bool bCanTick = false;
	
	UPROPERTY()
	float ElapsedTickTime = 0;
	
	UPROPERTY()
	UBSFiniteStateComponent* OwnerComponent;
};
