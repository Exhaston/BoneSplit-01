// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BSLockableInterface.h"
#include "GameFramework/Actor.h"
#include "BSDoor.generated.h"

UCLASS(Abstract, Blueprintable, BlueprintType)
class BONESPLIT_API ABSDoor : public AActor, public IBSLockableInterface
{
	GENERATED_BODY()

public:
	
	ABSDoor(const FObjectInitializer& ObjectInitializer);
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual void BeginPlay() override;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	bool bStartLocked = true;
	
	virtual void SetLockState(bool bLocked) override;
	
	virtual bool GetIsLocked() override { return bIsLocked; }
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_LockDoor)
	bool bIsLocked = true;
	
	UFUNCTION()
	void OnRep_LockDoor();
	
	UFUNCTION(BlueprintNativeEvent)
	void OnUnlocked();
	
	UFUNCTION(BlueprintNativeEvent)
	void OnLocked();
	
private:
	bool bLockStateSetExternally = false;
};
