// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "ClientAuthoritativeCharacter.h"
#include "GameplayTagAssetInterface.h"
#include "Factions/FactionInterface.h"
#include "BSPlayerCharacterBase.generated.h"

class UBSPlayerInteractionComponent;
class UBSPlayerAnimInstance;
struct FGameplayEventData;
class UBSScreenAlignedTextComponent;
class UTextRenderComponent;
class UCameraComponent;
class USpringArmComponent;
class UBSProjectileSpawnerComponent;
class UBSCharacterInitData;
class UBSEquipmentComponent;                  
class UBSAbilitySystem;

namespace BSConsoleVariables
{
	extern TAutoConsoleVariable<float> CVarBSCameraOffset;
	extern TAutoConsoleVariable<float> CVarBSCameraFieldOfView;
	extern TAutoConsoleVariable<bool> CVarDebugNoEquipment;
	
	extern TAutoConsoleVariable<bool> CVarBSShowOwnName;
	extern TAutoConsoleVariable<bool> CVarBSShowOtherNames;
}

UCLASS(Blueprintable, BlueprintType, Abstract, DisplayName="BS Player Character Base")
class BONESPLIT_API ABSPlayerCharacterBase : public AClientAuthoritativeCharacter, 
public IGameplayTagAssetInterface, public IAbilitySystemInterface, public IFactionInterface
{
	GENERATED_BODY()

public:
	
	explicit ABSPlayerCharacterBase(const FObjectInitializer& ObjectInitializer);
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual void BeginPlay() override;
	
	virtual void PossessedBy(AController* NewController) override;
	
	virtual void OnRep_Controller() override;
	
	virtual void OnRep_PlayerState() override;
	
	virtual void OnPlayerStateChanged(APlayerState* NewPlayerState, APlayerState* OldPlayerState) override;
	
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	
	virtual void Tick(float DeltaSeconds) override;
	
	virtual void TeleportToSafety();
	
	UFUNCTION(Client, Reliable)
	void Client_TeleportToSafety();
	
	//Attempts to initialize the ability system from the player state. 
	//Should be called from multiple events (Such as possessed, OnRep_Controller etc.), 
	//and is designed to ensure everything is valid before completion.
	virtual void InitAbilitySystem();
	
	FSimpleMulticastDelegate& GetOnAbilitySystemInitializedDelegate() { return OnAbilitySystemInitializedDelegate; }
	bool GetIsAbilitySystemInitComplete() const { return bAbilitySystemInitialized; }
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;
	virtual bool HasMatchingGameplayTag(FGameplayTag TagToCheck) const override;
	virtual bool HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;
	virtual bool HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;
	
	virtual FGameplayTagContainer& GetFactionTags() override;
	virtual bool HasAnyMatchingFactionTag(FGameplayTagContainer InFactionTags) override;
	void SetMenuCamera();
	void ResetCamera();
	
	virtual void LaunchCharacter(FVector LaunchVelocity, bool bXYOverride, bool bZOverride) override;

protected:
	
	virtual void TrySavePosition(float DeltaTime);
	bool IsOnNavMesh() const;

	FVector LastSafePos;
	
	UFUNCTION(Client, Reliable)
	void Client_LaunchCharacter(FVector Direction, bool OverrideXY, bool OverrideZ);
	
	virtual UBSPlayerAnimInstance* TryGetPlayerAnimInstance();
	
	virtual bool CanJumpInternal_Implementation() const override;
	
	virtual void OnAbilitySystemInitComplete();
	virtual void SetupPlayerName();
	
	virtual FVector GetTargetLocation(AActor* RequestedBy = nullptr) const override;
	
	virtual void OnSuccessfulBlock(FGameplayTag MatchingTag, const FGameplayEventData* EventData);
	
	FSimpleMulticastDelegate OnAbilitySystemInitializedDelegate;
	
	bool bAbilitySystemInitialized = false;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, ReplicatedUsing=OnRep_FactionTags)
	FGameplayTagContainer FactionTags;
	
	UFUNCTION()
	void OnRep_FactionTags();
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TObjectPtr<UBSCharacterInitData> CharacterInitData = nullptr;
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<UBSEquipmentComponent> EquipmentComponent;
	
	//Can be null during initialization, points to player state Asc.
	UPROPERTY(Transient, BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<UBSAbilitySystem> AbilitySystemComponent = nullptr;
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<UBSProjectileSpawnerComponent> ProjectileSpawnerComponent;
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<USpringArmComponent> SpringArmComponent;
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<UCameraComponent> CameraComponent;
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<UBSScreenAlignedTextComponent> PlayerNameTextComponent;
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<UAudioComponent> AudioComponent;
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<USceneComponent> TargetComponent;
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<UBSPlayerInteractionComponent> PlayerInteractionComponent;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TObjectPtr<UAnimMontage> BlockMontage = nullptr;
};
