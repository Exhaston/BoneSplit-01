// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BSLockableActor.generated.h"

UCLASS(DisplayName="Lockable Actor Base", Category="BoneSplit", ClassGroup="BoneSplit")
class BONESPLIT_API ABSLockableActor : public AActor
{
	GENERATED_BODY()

public:                                                                                 
	
	explicit ABSLockableActor(const FObjectInitializer& ObjectInitializer);
	
	virtual void NativeOnLocked();
	
	virtual void NativeOnUnLocked();
	
	//Native event that runs whenever this is unlocked by an external source. Wave Starters etc.
	UFUNCTION(BlueprintNativeEvent, DisplayName="On Unlocked")
	void BP_OnUnlocked();
	                    
	//Native event that runs whenever this is locked by an external source. Wave Starters etc.
	UFUNCTION(BlueprintNativeEvent, DisplayName="On Locked")
	void BP_OnLocked();
	
	//Returns true if currently locked
	UFUNCTION(BlueprintPure)
	bool IsLocked() const;
	
protected:
	
	//Set to true if this should start locked. A wave starter may unlock this if included.
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	bool bLocked = true;
};
