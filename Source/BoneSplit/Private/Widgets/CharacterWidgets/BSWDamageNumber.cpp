// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Widgets/CharacterWidgets/BSWDamageNumber.h"

#include "BoneSplit/BoneSplit.h"

void UBSWDamageNumber::InitializeDamageNumber(FGameplayEventData EventData)
{
	WorldLocation = EventData.Target->GetActorLocation();
	RandomScreenOffset = {FMath::RandRange(-100.f, 100.f), 0};
	SetLocation();
	RichDamageText->SetText(FText::FromString(FString::FromInt(FMath::RoundToInt(EventData.EventMagnitude))));
	
	UWidgetAnimation* AnimToPlay = nullptr;
	
	if (EventData.InstigatorTags.HasTagExact(BSTags::EffectTag_Critical))
	{
		AnimToPlay = CritAnimation;
	}
	if (!AnimToPlay && EventData.InstigatorTags.HasTagExact(BSTags::EffectTag_Important))
	{
		AnimToPlay = ImpactAnimation;
	}
	if (!AnimToPlay)
	{
		AnimToPlay = FloatAnimation;
	}
	
	FWidgetAnimationHandle Handle = PlayAnimation(AnimToPlay);
	Handle.GetAnimationState()->GetOnWidgetAnimationFinished().AddWeakLambda(this, [this]
		(FWidgetAnimationState& State)
	{
		RemoveFromParent();
	});
}

void UBSWDamageNumber::NativeTick(const FGeometry& MyGeometry, const float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	
	if (!IsValid(this)) return;
	SetLocation();
}

void UBSWDamageNumber::SetLocation()
{
	FVector2D ScreenLocation;
	GetOwningPlayer()->ProjectWorldLocationToScreen(WorldLocation, ScreenLocation, true);
	SetPositionInViewport(ScreenLocation + RandomScreenOffset);
}
