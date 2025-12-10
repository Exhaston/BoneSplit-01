// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "GameSettings/BSDeveloperSettings.h"

#if WITH_EDITOR

void UBSDeveloperSettings::PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent)
{
	Super::PostEditChangeChainProperty(PropertyChangedEvent);

	for (auto& SeasonalEvent : SeasonalEvents)
	{
		SeasonalEvent.StartMonth = FMath::Clamp(SeasonalEvent.StartMonth, 1, 12);
		SeasonalEvent.EndMonth   = FMath::Clamp(SeasonalEvent.EndMonth,   1, 12);
		
		const int32 MaxStartDays = GetDaysInMonth(SeasonalEvent.StartMonth);
		const int32 MaxEndDays   = GetDaysInMonth(SeasonalEvent.EndMonth);
		
		SeasonalEvent.StartDay = FMath::Clamp(SeasonalEvent.StartDay, 1, MaxStartDays);
		SeasonalEvent.EndDay   = FMath::Clamp(SeasonalEvent.EndDay,   1, MaxEndDays);
	}
}

int32 UBSDeveloperSettings::GetDaysInMonth(const int32 Month)
{
	static const int32 Days[12] = { 31,28,31,30,31,30,31,31,30,31,30,31 };
	return Days[FMath::Clamp(Month - 1, 0, 11)];
}

#endif
