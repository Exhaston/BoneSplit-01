// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "BSCalendarEventSubsystem.generated.h"

USTRUCT(BlueprintType)
struct FBSSeasonalEvent
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FString EventName;

	UPROPERTY(EditAnywhere)
	int32 StartMonth = 1;

	UPROPERTY(EditAnywhere)
	int32 StartDay = 1;

	UPROPERTY(EditAnywhere)
	int32 EndMonth = 1;

	UPROPERTY(EditAnywhere)
	int32 EndDay = 1;
	
	UPROPERTY(EditAnywhere)
	FGameplayTag EventTag;
};
/**
 * 
 */
UCLASS()
class BONESPLIT_API UBSCalendarEventSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	
	static UBSCalendarEventSubsystem* Get(const UObject* WorldContext);
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	
	static bool IsCurrentInWrappedRange(int32 StartMonth, int32 StartDay, int32 EndMonth, int32 EndDay);
	static bool IsEventActive(const FBSSeasonalEvent& E);
	
	UFUNCTION(BlueprintPure)
	FGameplayTagContainer GetCurrentEventTags() const;

protected:
	UPROPERTY()
	FGameplayTagContainer CurrentEventTags;
};
