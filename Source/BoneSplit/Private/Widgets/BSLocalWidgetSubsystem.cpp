// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Widgets/BSLocalWidgetSubsystem.h"

#include "Actors/Player/BSPlayerState.h"
#include "GameSettings/BSDeveloperSettings.h"
#include "GameplayAbilities/Public/AbilitySystemComponent.h"

#define BS_LOAD_WIDGET_SYNC(Pointer, InSoftClass) \
if (!InSoftClass.IsNull())                     \
{ Pointer = InSoftClass.LoadSynchronous(); }

UBSLocalWidgetSubsystem::UBSLocalWidgetSubsystem()
{

}

UBSLocalWidgetSubsystem* UBSLocalWidgetSubsystem::GetWidgetSubsystem(const UObject* Context)
{
	const UWorld* World = GEngine->GetWorldFromContextObjectChecked(Context);
	return World->GetGameInstance()->GetSubsystem<UBSLocalWidgetSubsystem>();
}

void UBSLocalWidgetSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	const UBSDeveloperSettings* DS =
		GetDefault<UBSDeveloperSettings>();

	for (auto& OverrideClass : DS->WidgetOverrideClasses)
	{
		if (!OverrideClass.Key.IsValid()) continue;
		if (OverrideClass.Value.IsNull()) continue;
		if (UClass* LoadedClass = OverrideClass.Value.LoadSynchronous())
		{
			WidgetClassOverrides.Add({OverrideClass.Key, LoadedClass} );
		}
	}
}

void UBSLocalWidgetSubsystem::GetOrSetAbilitySystem(const UUserWidget* WidgetInstance, TWeakObjectPtr<UAbilitySystemComponent>& PointerToSet)
{
	if (const IAbilitySystemInterface* AbilitySystemInterface = Cast<IAbilitySystemInterface>(WidgetInstance->GetOwningPlayerState()))
	{
		PointerToSet = AbilitySystemInterface->GetAbilitySystemComponent();
	}
}

bool UBSLocalWidgetSubsystem::IsWidgetActive(UCommonActivatableWidgetStack* StackToCheck, TSubclassOf<UCommonActivatableWidget> WidgetClass)
{
	check(WidgetClass);
	return StackToCheck->GetWidgetList().ContainsByPredicate(
	[WidgetClass](const UCommonActivatableWidget* Widget)
	{
		return Widget->IsA(WidgetClass);
	}); 
}

void UBSLocalWidgetSubsystem::RemoveWidgetFromStack(UCommonActivatableWidgetStack* StackToCheck, const TSubclassOf<UCommonActivatableWidget> WidgetInstance)
{
	check(StackToCheck);
	for (const auto Widget : StackToCheck->GetWidgetList())
	{
		if (Widget->IsA(WidgetInstance)) Widget->DeactivateWidget();
		break;
	}
}
