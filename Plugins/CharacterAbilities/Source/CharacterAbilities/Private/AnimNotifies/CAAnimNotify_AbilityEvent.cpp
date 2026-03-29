// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "AnimNotifies/CAAnimNotify_AbilityEvent.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"

UCAAnimNotify_AbilityEvent::UCAAnimNotify_AbilityEvent()
{
#if WITH_EDITOR
	NotifyColor = FColor::Cyan;
#endif
}

#if WITH_EDITOR
FString UCAAnimNotify_AbilityEvent::GetNotifyName_Implementation() const
{
	return "Ability Event";
}
#endif

void UCAAnimNotify_AbilityEvent::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	
	if (EventTag.IsValid()) return;
	
	AActor* Owner = MeshComp->GetOwner();
	
	if (!Owner || !Owner->HasLocalNetOwner()) return;
	
	if (const IAbilitySystemInterface* AscI = CastChecked<IAbilitySystemInterface>(Owner, ECastCheckedType::NullAllowed))
	{
		if (UAbilitySystemComponent* Asc = AscI->GetAbilitySystemComponent())
		{
			FGameplayEventData Payload;
			Payload.ContextHandle = Asc->MakeEffectContext();
			Payload.EventMagnitude = EventMagnitude;
			Payload.Instigator = Owner;
			Payload.EventTag = EventTag;
			Payload.Target = Owner;
			
			Asc->HandleGameplayEvent(EventTag, &Payload);
		}
	}
}
