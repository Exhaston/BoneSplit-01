// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Threat/MThreatInterface.h"
#include "MMobCharacter.generated.h"

class ANavigationData;
class AMMobPatrolPath;
class AMMobController;

UCLASS(DisplayName="Mob Character Base", Blueprintable, BlueprintType, Abstract)
class MOBS_API AMMobCharacter : public ACharacter, public IMThreatInterface
{
	GENERATED_BODY()

public:
	
	explicit AMMobCharacter(const FObjectInitializer& ObjectInitializer);
	
	virtual void PostInitializeComponents() override;
	
	virtual void PossessedBy(AController* NewController) override;
	
	virtual void LaunchCharacter(FVector LaunchVelocity, bool bXYOverride, bool bZOverride) override;
	
	virtual void OnFocusTargetChanged(AActor* NewFocus);
	
	virtual void OnTargetChanged(AActor* OldTarget, AActor* NewTarget);
	
	UFUNCTION()
	virtual void OnNavigationReady();
	
	virtual void NativeOnMobTick(float DeltaSeconds);
	
	UFUNCTION(BlueprintNativeEvent, DisplayName="OnMobTick")
	void BP_OnMobTick(float DeltaSeconds);
	
	UFUNCTION(BlueprintPure)
	AMMobController* GetMobController();
	
	virtual UMThreatComponent* GetThreatComponent() override;
	
protected:
	
	UPROPERTY()
	FTimerHandle MobNavMeshTimer;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Mob")
	float MoveToThreatAcceptanceRadius = 150;
	
	UPROPERTY(BlueprintReadOnly, EditInstanceOnly, Category="Mob")
	TObjectPtr<AMMobPatrolPath> MobPatrolPath = nullptr;
};
