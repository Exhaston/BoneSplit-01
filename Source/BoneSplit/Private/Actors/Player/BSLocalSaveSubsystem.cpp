// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Actors/Player/BSLocalSaveSubsystem.h"

#include "AbilitySystemComponent.h"
#include "GameSettings/BSDeveloperSettings.h"
#include "Kismet/GameplayStatics.h"

void UBSLocalSaveSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	const UBSDeveloperSettings* DeveloperSettings = GetDefault<UBSDeveloperSettings>();
	SaveGameClass = DeveloperSettings->SaveGameClass.LoadSynchronous();
}

const UBSSaveGame* UBSLocalSaveSubsystem::GetSaveGameCDO() const
{
	return GetDefault<UBSSaveGame>(SaveGameClass);
}

TSubclassOf<UBSSaveGame> UBSLocalSaveSubsystem::GetSaveGameClass()
{
	return SaveGameClass;
}

bool UBSLocalSaveSubsystem::GetSaveGameNames(TArray<FString>& FoundSaves, const int32 UserIndex)
{
	TArray<FString> FoundFiles;
	const FString SaveGameDirectory = FPaths::ProjectSavedDir() / TEXT("SaveGames/");
	IFileManager::Get().FindFiles(FoundFiles, *SaveGameDirectory, TEXT("*.sav"));

	for (FString& File : FoundFiles)
	{
		FoundSaves.Add(FPaths::GetBaseFilename(File));
	}

	return true;
}

UBSSaveGame* UBSLocalSaveSubsystem::GetOrCreateSaveGameInstance(const APlayerController* PlayerController)
{
	if (SaveGameInstance)
	{
		return SaveGameInstance.Get(); //Fast already cached save object loaded into memory
	}
	if (UGameplayStatics::DoesSaveGameExist(CurrentProfile, PlayerController->GetLocalPlayer()->GetPlatformUserIndex()))
	{
		//We can load a valid save into memory
		SaveGameInstance = Cast<UBSSaveGame>(UGameplayStatics::LoadGameFromSlot(CurrentProfile, 
		PlayerController->GetLocalPlayer()->GetPlatformUserIndex()));
	}
	else
	{     
		//No saves found, creating a new one as a fallback...
		SaveGameInstance = Cast<UBSSaveGame>(UGameplayStatics::CreateSaveGameObject(SaveGameClass));
	}
	
	return SaveGameInstance.Get();
}

FBSSaveData& UBSLocalSaveSubsystem::GetOrCreateSaveData(const APlayerController* PlayerController)
{
	return GetOrCreateSaveGameInstance(PlayerController)->SaveData;
}

void UBSLocalSaveSubsystem::SaveAscDataSync(APlayerController* PlayerController, UAbilitySystemComponent* Asc)
{
	if (!PlayerController->IsLocalController()) return;
	FBSSaveData& SaveData = GetOrCreateSaveData(PlayerController);
	SaveAscData(Asc, SaveData);
	SaveGameSync(PlayerController);
}

void UBSLocalSaveSubsystem::SaveAscDataAsync(APlayerController* PlayerController, UAbilitySystemComponent* Asc)
{
	if (!PlayerController->IsLocalController()) return;
	FBSSaveData& SaveData = GetOrCreateSaveData(PlayerController);
	SaveAscData(Asc, SaveData);
	SaveGameAsync(PlayerController);
}

void UBSLocalSaveSubsystem::ApplyDefaultData(UAbilitySystemComponent* Asc)
{
	if (!Asc->IsOwnerActorAuthoritative()) return;
	const UBSSaveGame* SG_CDO = GetSaveGameCDO();
	for (const auto DefaultAbility : SG_CDO->DefaultAbilities)
	{
		Asc->GiveAbility(DefaultAbility);
	}

	for (const auto DefaultEffect : SG_CDO->DefaultEffects)
	{
		FGameplayEffectSpecHandle EffectSpec = Asc->MakeOutgoingSpec(
			DefaultEffect, 1, Asc->MakeEffectContext());

		Asc->ApplyGameplayEffectSpecToSelf(*EffectSpec.Data);
	}
		
	
	for (auto NewTag : SG_CDO->DefaultTags)
	{
		Asc->AddLooseGameplayTag(
			NewTag.Key, NewTag.Value, EGameplayTagReplicationState::TagAndCountToAll);
	}
}

void UBSLocalSaveSubsystem::PopulateAscFromSaveData(UAbilitySystemComponent* Asc, const FBSSaveData& Data)
{
	if (!Asc->IsOwnerActorAuthoritative()) return;
	for (auto ExistingAttribute : Data.Attributes)
	{
		//Override the base of an attribute to what was stored in the save.
		Asc->SetNumericAttributeBase(ExistingAttribute.Attribute, ExistingAttribute.Value);
	}
	
	for (const auto ExistingEffect : Data.Effects)
	{
		//Restore Effect from class
		const FGameplayEffectContextHandle Context = Asc->MakeEffectContext();

		FGameplayEffectSpecHandle SpecHandle = Asc->MakeOutgoingSpec(
			ExistingEffect.EffectClass,
			1,
			Context
		);

		if (!SpecHandle.IsValid()) continue;

		// Restore stack count
		SpecHandle.Data->SetStackCount(ExistingEffect.StackCount);

		// Timed effect
		if (ExistingEffect.RemainingTime != 0)
		{
			SpecHandle.Data->SetDuration(
				ExistingEffect.RemainingTime < 0 ? -1 : ExistingEffect.RemainingTime, true);
		}

		Asc->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	}
	
	/*
	for (auto NewTag : Data.Tags)
	{
		Asc->AddLooseGameplayTag(NewTag, 1, EGameplayTagReplicationState::TagAndCountToAll);
	}
	*/
}

void UBSLocalSaveSubsystem::SetProfileName(FString InProfileName)
{
	CurrentProfile = InProfileName;
}

void UBSLocalSaveSubsystem::SaveAscData(UAbilitySystemComponent* Asc, FBSSaveData& Data)
{
	//Save attribute bases
	TArray<FGameplayAttribute> CurrentAttributes;
	Asc->GetAllAttributes(CurrentAttributes);

	Data.Attributes.Empty();
	
	for (auto Attribute : CurrentAttributes)
	{
		FBSAttributeData AttributeData;
		AttributeData.Attribute = Attribute;
		//Only get the base. Current would cause conflicts with temporary effects.
		AttributeData.Value = Asc->GetNumericAttributeBase(Attribute);
		Data.Attributes.Add(AttributeData);
	}
	
	//Save Effects (talents, equipment, others)
	
	Data.Effects.Empty();
	const FGameplayEffectQuery Query = FGameplayEffectQuery::MakeQuery_MatchNoEffectTags(FGameplayTagContainer());
	
	for (TArray<FActiveGameplayEffectHandle> ActiveHandles = Asc->GetActiveEffects(Query);
		const FActiveGameplayEffectHandle& Handle : ActiveHandles)
	{
		
		const FActiveGameplayEffect* ActiveEffect = Asc->GetActiveGameplayEffect(Handle);
		
		if (!ActiveEffect) continue;
		
		if (ActiveEffect->Spec.Def->DurationPolicy == EGameplayEffectDurationType::Instant)
		{
			continue;
		}
		
		bool bSaveable = false;
		
		for (const auto SaveableEffectClass : GetSaveGameCDO()->SaveableEffectClasses)
		{
			if (ActiveEffect->Spec.Def->IsA(SaveableEffectClass))
			{
				bSaveable = true;
				break;
			}
		}
		
		if (!bSaveable) continue;

		FBSGameplayEffectData EffectData;
		EffectData.RemainingTime = -1; //Init as infinite
		EffectData.EffectClass = ActiveEffect->Spec.Def->GetClass();
		EffectData.StackCount = ActiveEffect->Spec.GetStackCount();

		// Get remaining duration (if it’s a timed effect)
		if (ActiveEffect->GetDuration() > 0)
		{
			EffectData.RemainingTime = 
				ActiveEffect->GetTimeRemaining(Asc->GetWorld()->GetTimeSeconds());
		}

		Data.Effects.Add(EffectData);
	}
}

void UBSLocalSaveSubsystem::LoadGameAsync(const APlayerController* PlayerController)
{
	if (!PlayerController->IsLocalController()) return;
	if (SaveGameInstance)
	{
		OnAsyncLoadComplete.Broadcast(SaveGameInstance);
	}
	if (UGameplayStatics::DoesSaveGameExist(CurrentProfile, PlayerController->GetLocalPlayer()->GetPlatformUserIndex()))
	{
		//We can load a valid save into memory
		UGameplayStatics::AsyncLoadGameFromSlot(CurrentProfile, 
		PlayerController->GetLocalPlayer()->GetPlatformUserIndex(), 
		FAsyncLoadGameFromSlotDelegate::CreateWeakLambda(this, [this]
		(const FString& SaveSlot, const int32 UserIndex, USaveGame* SaveGame)
		{
				SaveGameInstance = Cast<UBSSaveGame>(SaveGame);
				OnAsyncLoadComplete.Broadcast(SaveGameInstance);
		}));
	}
	else
	{     
		//No saves found, creating a new one as a fallback...
		SaveGameInstance = Cast<UBSSaveGame>(UGameplayStatics::CreateSaveGameObject(SaveGameClass));
		OnAsyncLoadComplete.Broadcast(SaveGameInstance);
	}
}

void UBSLocalSaveSubsystem::SaveGameSync(const APlayerController* PlayerController) const
{
	if (!SaveGameInstance) return;
	UGameplayStatics::SaveGameToSlot(
		SaveGameInstance.Get(), CurrentProfile, PlayerController->GetLocalPlayer()->GetPlatformUserIndex());
}

void UBSLocalSaveSubsystem::SaveGameAsync(const APlayerController* PlayerController)
{
	UGameplayStatics::AsyncSaveGameToSlot(
	GetOrCreateSaveGameInstance(PlayerController), 
	CurrentProfile, 
	PlayerController->GetLocalPlayer()->GetPlatformUserIndex(),
	FAsyncSaveGameToSlotDelegate::CreateWeakLambda(this,[this]
	(const FString& SaveSlot, const int32 PlayerNum, bool Success)
	{
		if (Success)
		{
			OnAsyncSaveComplete.Broadcast();
		}
	}));
}


