// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BSPowerUpComponent.generated.h"

class ABSPowerUpPickup;
class UAbilitySystemComponent;
struct FBSPowerUpRow;
class UBSPowerUpPool;
class UBSPowerUpData;

USTRUCT(BlueprintType)
struct FBSAvailableChoice
{
	GENERATED_BODY()
	
	UPROPERTY()
	FGuid PowerUpGuid;
	
	UPROPERTY()
	TArray<TObjectPtr<UBSPowerUpData>> ChoiceData;
	
	bool operator==(const FBSAvailableChoice& Other) const
	{
		return PowerUpGuid == Other.PowerUpGuid;
	}
};


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class BONESPLIT_API UBSPowerUpComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	
	UBSPowerUpComponent();
	
	static UBSPowerUpComponent* GetPowerUpComponent(APlayerState* OwnerPS);
	
	virtual void SpawnPowerUpForPlayer(FVector Location, int32 MaxChoices = 1);

	static UBSPowerUpData* PickWeightedRandom(const TArray<FBSPowerUpRow>& EligibleRows, float TotalWeight);
	
	UFUNCTION(Client, Reliable)
	void Client_SpawnPowerUp(FVector Location, const FBSAvailableChoice& InChoice);
	
	virtual void ClaimPowerUp(const FBSAvailableChoice& InChoice, int32 ChoiceIndex);
	
	UFUNCTION(Server, Reliable)
	void Server_ClaimPowerUp(const FBSAvailableChoice& Choice, int32 Index);
	
	//Transfers the power ups of this component onto a new one.
	virtual void TransferPowerUps(UBSPowerUpComponent* NewComponent);
	
	UPROPERTY()
	TArray<FBSAvailableChoice> AvailableChoices;
	
	UPROPERTY()
	TMap<UBSPowerUpData*, int32> NumGiven;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TSubclassOf<ABSPowerUpPickup> PowerUpClass;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TObjectPtr<UBSPowerUpPool> PoolData;
	
};
