// Copyright 2023 Anton Hesselbom. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ClientAuthoritativeMoveData.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ClientAuthoritativeCharacterMovementComponent.generated.h"

// Intended to be used together with AClientAuthoritativeCharacter & AClientAuthoritativePlayerController
UCLASS()
class CLIENTAUTHORITATIVECHARACTERSYSTEM_API UClientAuthoritativeCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	UClientAuthoritativeCharacterMovementComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void SimulatedTick(float DeltaSeconds) override;
	virtual void ControlledCharacterMove(const FVector& InputVector, float DeltaSeconds) override;
	virtual void SimulateMovement(float DeltaTime) override;
	virtual void SmoothClientPosition(float DeltaSeconds) override;
	virtual void SmoothCorrection(const FVector& OldLocation, const FQuat& OldRotation, const FVector& NewLocation, const FQuat& NewRotation) override;

	void ClientAuthoritativeMove_ServerReceive(const FClientAuthoritativeMoveData& MoveData);

	FORCEINLINE const FClientAuthoritativeMoveData& GetServerLatestMoveData() const { return ServerLatestMoveData; }

private:
	void ReplicateAutonomousMoveToServer();
	float GetClientAuthoritativeNetSendDeltaTime(const APlayerController* Controller) const;
	void SendClientAuthoritativeMove();
	FClientAuthoritativeMoveData GenerateClientAuthoritativeMoveData();

	float LastClientUpdateTime;

	UPROPERTY()
	FClientAuthoritativeMoveData ServerLatestMoveData;
};
