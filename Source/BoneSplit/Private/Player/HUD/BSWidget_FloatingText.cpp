// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Player/HUD/BSWidget_FloatingText.h"

#include "CommonRichTextBlock.h"
#include "BoneSplit/BoneSplit.h"

namespace BSGameplayTags
{
	UE_DEFINE_GAMEPLAY_TAG(Widget_FloatingText_Default, "Widget.FloatingText.Default");
	UE_DEFINE_GAMEPLAY_TAG(Widget_FloatingText_Helpful, "Widget.FloatingText.Helpful");
	UE_DEFINE_GAMEPLAY_TAG(Widget_FloatingText_Crit, "Widget.FloatingText.Crit");
	UE_DEFINE_GAMEPLAY_TAG(Widget_FloatingText_Impact, "Widget.FloatingText.Impact");
}

void UBSWidget_FloatingText::SetFloatingText(const FVector InWorldLocation, const FText& Text,
	const FGameplayTagContainer& RelevantTags)
{
	WorldLocation = InWorldLocation;
	
	RandomScreenOffset = 
		{FMath::RandRange(-RandomOffsetBounds.X, RandomOffsetBounds.X), 
		FMath::RandRange(-RandomOffsetBounds.Y, RandomOffsetBounds.Y)};
	
	RichDamageText->SetText(Text);
	SetStyle(RelevantTags);
	
	ProjectToScreen();
}

void UBSWidget_FloatingText::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	ProjectToScreen();
}

void UBSWidget_FloatingText::ProjectToScreen()
{
	FVector2D ScreenLocation;
	GetOwningPlayer()->ProjectWorldLocationToScreen(WorldLocation, ScreenLocation, true);
	SetPositionInViewport(ScreenLocation + RandomScreenOffset);
}

void UBSWidget_FloatingText::SetStyle(const FGameplayTagContainer& RelevantTags)
{
	UWidgetAnimation* AnimToPlay;
	
	if (RelevantTags.HasTagExact(BSGameplayTags::Widget_FloatingText_Crit))
	{
		RichDamageText->SetStyle(CriticalStyle);
		AnimToPlay = CritAnimation;
	}
	else if (RelevantTags.HasTagExact(BSGameplayTags::Widget_FloatingText_Impact))
	{
		RichDamageText->SetStyle(ImpactStyle);
		AnimToPlay = ImpactAnimation;
	}
	else if (RelevantTags.HasTagExact(BSGameplayTags::Widget_FloatingText_Helpful))
	{
		RichDamageText->SetStyle(HelpfulStyle);
		AnimToPlay = FloatAnimation;
	}
	else
	{
		RichDamageText->SetStyle(PhysicalDamageStyle);
		AnimToPlay = FloatAnimation;
	}
	
	if (!AnimToPlay)
	{
		UE_LOG(BoneSplit, Error, TEXT("Floating Text: Class had no valid animations to play. Destroying."));
		RemoveFromParent();
		return;
	}
	
	PlayAnimation(AnimToPlay).GetAnimationState()->GetOnWidgetAnimationFinished().AddWeakLambda(
	this, [this] (FWidgetAnimationState& State)
	{
		RemoveFromParent();
	});
}
