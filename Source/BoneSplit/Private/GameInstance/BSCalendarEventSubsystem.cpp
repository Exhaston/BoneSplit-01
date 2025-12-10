// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "GameInstance/BSCalendarEventSubsystem.h"

#include "GameSettings/BSDeveloperSettings.h"

UBSCalendarEventSubsystem* UBSCalendarEventSubsystem::Get(const UObject* WorldContext)
{
	const UWorld* World = GEngine->GetWorldFromContextObjectChecked(WorldContext);
	const UGameInstance* GameInstance = World->GetGameInstanceChecked<UGameInstance>();
	return GameInstance->GetSubsystem<UBSCalendarEventSubsystem>();
}

void UBSCalendarEventSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	const UBSDeveloperSettings* DeveloperSettings = GetDefault<UBSDeveloperSettings>();
	for (auto Event : DeveloperSettings->SeasonalEvents)
	{
		if (IsEventActive(Event))
		{
			CurrentEventTags.AddTag(Event.EventTag);
		}
	}
}

bool UBSCalendarEventSubsystem::IsCurrentInWrappedRange(
	const int32 StartMonth, const int32 StartDay, const int32 EndMonth, const int32 EndDay)
{
	return (EndMonth < StartMonth) || (EndMonth == StartMonth && EndDay < StartDay);
}

bool UBSCalendarEventSubsystem::IsEventActive(const FBSSeasonalEvent& E)
{
	const FDateTime Today = FDateTime::Now();
	const int32 Year = Today.GetYear();

	const FDateTime StartThisYear(Year, E.StartMonth, E.StartDay);
	const FDateTime EndThisYear(Year, E.EndMonth, E.EndDay);
	
	if (IsCurrentInWrappedRange(E.StartMonth, E.StartDay, E.EndMonth, E.EndDay))
	{
		// Example: winter (Dec -> Jan)
		if (Today >= StartThisYear || Today <= FDateTime(Year, E.EndMonth, E.EndDay))
			return true;
	}

	return Today >= StartThisYear && Today <= EndThisYear;
}

FGameplayTagContainer UBSCalendarEventSubsystem::GetCurrentEventTags() const
{
	return CurrentEventTags;
}
