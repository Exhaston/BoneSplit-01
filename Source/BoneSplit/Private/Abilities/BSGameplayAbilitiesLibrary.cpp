// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Abilities/BSGameplayAbilitiesLibrary.h"

#include "AIController.h"
#include "CharacterAbilitySystem.h"
#include "CharacterAbilities/Public/CharacterAbilityBase.h"
#include "Abilities/BSBuffEffect.h"
#include "Abilities/BSCharacterInitData.h"
#include "Abilities/BSDamageEffect.h"
#include "Abilities/BSExtendedAttributeSet.h"
#include "Abilities/BSHealingEffect.h"
#include "Factions/FactionInterface.h"

FVector UBSGameplayAbilitiesLibrary::ProjectToGround(UObject* ContextObject, FVector InLocation)
{
	UWorld* World = GEngine->GetWorldFromContextObject(ContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!World) return InLocation;

	GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Green, TEXT("DID IT"));
	FHitResult Hit;
	FVector TraceStart = InLocation;
	FVector TraceEnd   = InLocation - FVector(0.f, 0.f, 10000.f);

	FCollisionQueryParams Params;
	Params.bTraceComplex = true;

	if (World->LineTraceSingleByObjectType(Hit, TraceStart, TraceEnd, ECC_WorldStatic, Params))
		return Hit.ImpactPoint;

	return InLocation; // fallback — nothing hit
}

FVector UBSGameplayAbilitiesLibrary::GetTargetLocationFromActor(AActor* Actor)
{
	if (!ensure(Actor)) return FVector::ZeroVector;
	return Actor->GetTargetLocation();
}

void UBSGameplayAbilitiesLibrary::ApplyCharacterDataTo(UBSCharacterInitData* CharacterInitData,
                                                       UCharacterAbilitySystem* CharacterAbilitySystem, const int32 InLevel)
{
	if (!CharacterInitData || !CharacterAbilitySystem->IsOwnerActorAuthoritative()) return;
	
	for (auto& DefaultAbilityClass : CharacterInitData->GrantedDefaultAbilities)
	{
		FGameplayAbilitySpec NewSpec = FGameplayAbilitySpec(DefaultAbilityClass);
		NewSpec.Level = InLevel;
		CharacterAbilitySystem->GiveAbility(NewSpec);
	}
	
	for (const auto& DefaultEffectClass : CharacterInitData->GrantedEffects)
	{
		FGameplayEffectSpecHandle DefaultEffectHandle = 
			CharacterAbilitySystem->MakeOutgoingSpec(
				DefaultEffectClass, 
				InLevel, 
				CharacterAbilitySystem->MakeEffectContext());
				
		CharacterAbilitySystem->ApplyGameplayEffectSpecToSelf(*DefaultEffectHandle.Data);
	}
	
	for (auto& AttributeInfoPair : CharacterInitData->AttributeMap)
	{
		CharacterAbilitySystem->ApplyModToAttribute(
			AttributeInfoPair.Key, 
			EGameplayModOp::Override, 
			AttributeInfoPair.Value.GetValueAtLevel(InLevel));
	}
	
	CharacterAbilitySystem->AddLooseGameplayTags(CharacterInitData->FactionTags);
}

FRotator UBSGameplayAbilitiesLibrary::GetLookToTargetRotation(APawn* MobPawn, const bool bOnlyPitch)
{
	const AAIController* OwnerController = MobPawn->GetController<AAIController>();
	if (!OwnerController) return FRotator::ZeroRotator;
	const AActor* TargetActor = OwnerController->GetFocusActorForPriority(EAIFocusPriority::Gameplay);
	if (!TargetActor) return FRotator::ZeroRotator;

	const FVector Dir = (TargetActor->GetTargetLocation() - MobPawn->GetTargetLocation()).GetSafeNormal();
	
	FRotator FinalRot = Dir.Rotation();
	
	if (bOnlyPitch)
	{
		FinalRot.Yaw = MobPawn->GetActorRotation().Yaw;
	}
	return FinalRot;
}

bool UBSGameplayAbilitiesLibrary::PassFactionCheck(AActor* InActor, AActor* InTarget, bool bHitFriendly, bool bHitEnemy,
	bool bHitSelf)
{
	if (!InActor || !InTarget) return false;

	IFactionInterface* OwnerInterface = Cast<IFactionInterface>(InActor);
	IFactionInterface* TargetInterface = Cast<IFactionInterface>(InTarget);
	if (!OwnerInterface || !TargetInterface) return false;

	// Check self first — before faction tag comparison
	if (InActor == InTarget) return bHitSelf;

	bool bIsFriendly = TargetInterface->HasAnyMatchingFactionTag(OwnerInterface->GetFactionTags());

	if (bIsFriendly) return bHitFriendly;
	return bHitEnemy;
}

bool UBSGameplayAbilitiesLibrary::ApplyDamageTo(
	UAbilitySystemComponent* Instigator, 
	UAbilitySystemComponent* Target,
	const bool CustomOrigin,
	const FVector Origin,
	const bool bHitSelf,
	const bool bHitAllies,
	const bool bHitEnemies,
	TSubclassOf<UBSDamageEffect> DamageEffectClass,
	const float Damage, 
	const float Level,
	const float Knockback, 
	FGameplayTagContainer EffectTags)
{
	if (!Target || !Instigator) return false;
	if (!Target->GetAvatarActor() || !Instigator->GetAvatarActor()) return false;
	
	if (const IGameplayTagAssetInterface* TagAssetInterface = Cast<IGameplayTagAssetInterface>(Target->GetAvatarActor()))
	{
		FGameplayTagContainer OwnedTags;
		TagAssetInterface->GetOwnedGameplayTags(OwnedTags);
		if (OwnedTags.HasTagExact(BSExtendedAttributeTags::Status_God)) return false;
	}
	
	if (!DamageEffectClass)
	{
		DamageEffectClass = UBSDamageEffect::StaticClass();
	}
	
	if (!PassFactionCheck(
		Instigator->GetAvatarActor(), 
		Target->GetAvatarActor(), 
		bHitAllies, 
		bHitEnemies, 
		bHitSelf)) return false;
	
	if (Instigator->IsOwnerActorAuthoritative())
	{
		const FGameplayEffectSpecHandle SpecHandle = 
			Instigator->MakeOutgoingSpec(DamageEffectClass, Level, Instigator->MakeEffectContext());
		
		FVector NewOrigin = CustomOrigin ? Origin : Instigator->GetAvatarActor()->GetActorLocation();
		
		SpecHandle.Data->GetContext().AddOrigin(NewOrigin);
	
		for (auto DynamicAssetTags : EffectTags)
		{
			SpecHandle.Data->AddDynamicAssetTag(DynamicAssetTags);
		}
		
		SpecHandle.Data.Get()->SetSetByCallerMagnitude(DefaultSetByCallerTags::SetByCaller_Damage, Damage);
	
		SpecHandle.Data.Get()->SetSetByCallerMagnitude(BSExtendedAttributeTags::SetByCaller_Knockback, Knockback);
	
		Instigator->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data, Target);
		
		/*
		for (auto GE : AdditionalEffects)
		{
			const FGameplayEffectSpecHandle NewSpec = 
				Instigator->MakeOutgoingSpec(GE, Level, Instigator->MakeEffectContext());
			
			NewSpec.Data->GetContext().AddOrigin(NewOrigin);
			
			for (auto DynamicAssetTags : EffectTags)
			{
				NewSpec.Data->AddDynamicAssetTag(DynamicAssetTags);
			}
			
			Instigator->ApplyGameplayEffectSpecToTarget(*NewSpec.Data, Target);
		}
		*/
	}
	
	return true;
}

bool UBSGameplayAbilitiesLibrary::ApplyHealingTo(
	UAbilitySystemComponent* Instigator, 
	UAbilitySystemComponent* Target,
	const bool CustomOrigin, 
	const FVector Origin,
	const bool bHitSelf,
	const bool bHitAllies,
	const bool bHitEnemies,
	TSubclassOf<UBSHealingEffect> HealingEffectClass, 
	const float Healing, 
	const float Level, 
	const float Knockback,
	FGameplayTagContainer EffectTags)
{
	if (!Target || !Instigator) return false;
	if (!Target->GetAvatarActor() || !Instigator->GetAvatarActor()) return false;
	
	if (!HealingEffectClass)
	{
		HealingEffectClass = UBSHealingEffect::StaticClass();
	}
	
	if (!PassFactionCheck(Instigator->GetAvatarActor(), Target->GetAvatarActor(), bHitAllies, bHitEnemies, bHitSelf)) return false;
	
	const FGameplayEffectSpecHandle SpecHandle = 
		Instigator->MakeOutgoingSpec(HealingEffectClass, Level, Instigator->MakeEffectContext());
	
	if (CustomOrigin)
	{
		SpecHandle.Data->GetContext().AddOrigin(Origin);
	}
	else if (Instigator->GetAvatarActor())
	{
		SpecHandle.Data->GetContext().AddOrigin(Instigator->GetAvatarActor()->GetActorLocation());
	}
	
	for (auto DynamicAssetTags : EffectTags)
	{
		SpecHandle.Data->AddDynamicAssetTag(DynamicAssetTags);
	}
	
	SpecHandle.Data.Get()->SetSetByCallerMagnitude(DefaultSetByCallerTags::SetByCaller_Healing, Healing);
	SpecHandle.Data.Get()->SetSetByCallerMagnitude(BSExtendedAttributeTags::SetByCaller_Knockback, Knockback);
	
	Instigator->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data, Target);
	
	return true;
}

bool UBSGameplayAbilitiesLibrary::ApplyBuffTo(
	UAbilitySystemComponent* Instigator, 
	UAbilitySystemComponent* Target,
	const bool CustomOrigin, 
	const FVector Origin,
	const bool bHitSelf,
	const bool bHitAllies,
	const bool bHitEnemies,
	TSubclassOf<UBSBuffEffect> BuffEffectClass,
	const float Duration, 
	const float Level, 
	FGameplayTagContainer EffectTags)
{
	if (!Target || !Instigator) return false;
	if (!Target->GetAvatarActor() || !Instigator->GetAvatarActor()) return false;
	
	if (Duration <= 0) return false;
	
	if (!BuffEffectClass)
	{
		BuffEffectClass = UBSBuffEffect::StaticClass();
	}
	
	if (!PassFactionCheck(Instigator->GetAvatarActor(), Target->GetAvatarActor(), bHitAllies, bHitEnemies, bHitSelf)) return false;
	
	const FGameplayEffectSpecHandle SpecHandle = 
		Instigator->MakeOutgoingSpec(BuffEffectClass, Level, Instigator->MakeEffectContext());
	
	if (CustomOrigin)
	{
		SpecHandle.Data->GetContext().AddOrigin(Origin);
	}
	else if (Instigator->GetAvatarActor())
	{
		SpecHandle.Data->GetContext().AddOrigin(Instigator->GetAvatarActor()->GetActorLocation());
	}
	
	for (auto DynamicAssetTags : EffectTags)
	{
		SpecHandle.Data->AddDynamicAssetTag(DynamicAssetTags);
	}
	
	SpecHandle.Data.Get()->SetSetByCallerMagnitude(DefaultSetByCallerTags::SetByCaller_Duration, Duration);
	
	Instigator->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data, Target);
	
	return true;
}
