// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/PawnComponent.h"
#include "PawnInitializationComponent.generated.h"


class UCharacterAbilitySystem;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GAMEPLAYABILITIESEXTENSION_API UPawnInitializationComponent : public UPawnComponent
{
	GENERATED_BODY()

public:
	
	explicit UPawnInitializationComponent(const FObjectInitializer& ObjectInitializer);
	
	/** Returns the pawn extension component if one exists on the specified actor. */
	UFUNCTION(BlueprintPure)
	static UPawnInitializationComponent* FindPawnExtensionComponent(const AActor* Actor)
	{
		return Actor ? Actor->FindComponentByClass<UPawnInitializationComponent>() : nullptr;
	}
	
	UFUNCTION(BlueprintPure)
	UCharacterAbilitySystem* GetCharacterAbilitySystem() const { return AbilitySystemComponent; }
	
	virtual bool TryInitAbilitySystemForPlayer(UCharacterAbilitySystem* CharacterAbilitySystem, AActor* TargetOwnerActor);
	virtual bool TryInitAbilitySystemForAI(UCharacterAbilitySystem* CharacterAbilitySystem);
	virtual void HandleControllerChanged();

	/** Should be called by the owning pawn to remove itself as the avatar of the ability system. */
	void UninitializeAbilitySystem();

	/** Register with the OnAbilitySystemInitialized delegate and broadcast if our pawn has been registered with the ability system component */
	void OnAbilitySystemInitialized_RegisterAndCall(FSimpleMulticastDelegate::FDelegate Delegate);

	/** Register with the OnAbilitySystemUninitialized delegate fired when our pawn is removed as the ability system's avatar actor */
	void OnAbilitySystemUninitialized_Register(FSimpleMulticastDelegate::FDelegate Delegate);

	/** Delegate fired when our pawn becomes the ability system's avatar actor */
	FSimpleMulticastDelegate OnAbilitySystemInitialized;

	/** Delegate fired when our pawn is removed as the ability system's avatar actor */
	FSimpleMulticastDelegate OnAbilitySystemUninitialized;

	/** Pointer to the ability system component that is cached for convenience. */
	UPROPERTY(Transient)
	TObjectPtr<UCharacterAbilitySystem> AbilitySystemComponent;
};
