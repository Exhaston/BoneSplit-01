// Copyright 2023 Anton Hesselbom. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask_ApplyRootMotion_Base.h"
#include "GameFramework/RootMotionSource.h"
#include "ClientAuthoritativeAbilityTask.generated.h"

#define CA_ABILITYTASK_BOILERPLATE(Type)                                  \
	Type* MyTask = NewAbilityTask<Type>(OwningAbility, TaskInstanceName); \
	MyTask->ForceName = TaskInstanceName;                                 \
	MyTask->FinishVelocityMode = VelocityOnFinishMode;                    \
	MyTask->FinishSetVelocity = SetVelocityOnFinish;                      \
	MyTask->FinishClampVelocity = ClampVelocityOnFinish;

#define CA_ROOT_MOTION_SOURCE_BOILERPLATE(ClassName)                                      \
public:                                                                                \
	virtual FRootMotionSource* Clone() const override { return new ClassName(*this); } \
	virtual UScriptStruct* GetScriptStruct() const override { return ClassName::StaticStruct(); }

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FClientAuthoritativeAbilityTaskDelegate);

USTRUCT()
struct CLIENTAUTHORITATIVECHARACTERSYSTEM_API FClientAuthoritativeRootMotionSource : public FRootMotionSource
{
	GENERATED_BODY()

public:
	FClientAuthoritativeRootMotionSource();
	virtual void PrepareRootMotion(float SimulationTime, float MovementTickTime, const ACharacter& Character, const UCharacterMovementComponent& MoveComponent) override;

	virtual void OnStart(const ACharacter* Character, const UCharacterMovementComponent* MovementComponent) {}
	virtual FVector Update(float DeltaSeconds, const ACharacter* Character, const UCharacterMovementComponent* MovementComponent) { return FVector::ZeroVector; }

protected:
	void FinishMovement();

private:
	bool bIsSetup = false;
};


UCLASS()
class CLIENTAUTHORITATIVECHARACTERSYSTEM_API UClientAuthoritativeAbilityTask : public UAbilityTask_ApplyRootMotion_Base
{
	GENERATED_BODY()
	
public:
	virtual void Activate() override;
	virtual void TickTask(float DeltaTime);
	virtual void OnDestroy(bool AbilityIsEnding) override;

	UPROPERTY(BlueprintAssignable)
	FClientAuthoritativeAbilityTaskDelegate OnFinish;

protected:
	virtual FClientAuthoritativeRootMotionSource* CreateRootMotionSource() { return nullptr; }
	virtual void OnStart() {}
	virtual void OnStop() {}

private:
	bool bIsFinished;
};
