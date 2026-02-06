// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "BSSaveGame.h"
#include "GameFramework/PlayerState.h"
#include "BSPlayerState.generated.h"

class UBSTalentComponent;
class UBSAttributeSet;
struct FBSSaveData;
class UBSAbilitySystemComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FBSPlayerStateDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBSOnPlayerColorChanged, FColor, NewColor);

UCLASS(DisplayName="Player State", BlueprintType, Blueprintable, Category="BoneSplit")
class BONESPLIT_API ABSPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:
	
	explicit ABSPlayerState(const FObjectInitializer& ObjectInitializer);
	
	virtual void BeginPlay() override;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	//Send the save data to the server to initialize the Asc.
	UFUNCTION(Server, Reliable)
	void Server_ReceiveSaveData(const FBSSaveData& SaveData);
	
	//Sets the player color and triggers OnPlayerColorChanged Delegate for server and clients.
	UFUNCTION(Server, Reliable)                    
	void Server_SetColor(const int32& NewColor);
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	UFUNCTION(BlueprintPure)
	UBSTalentComponent* GetTalentComponent() const;
	
	//Checks if the server received save data and has set up Asc.
	bool GetIsInitialized() const;
	
	UFUNCTION(BlueprintPure)
	FColor GetPlayerColor() const;
	
	FBSPlayerStateDelegate& GetInitCompleteDelegate()   { return OnInitComplete; }
	FBSOnPlayerColorChanged& GetOnPlayerColorChanged()  { return OnPlayerColorChangedDelegate; }
	
	UBSAbilitySystemComponent* GetBSAbilitySystem() const;
	
	UFUNCTION(Server, Reliable)
	void Server_ReceiveWantPause();
	
	UFUNCTION(Server, Reliable)
	void Server_ReceiveWantResume();
	
protected:
	
	void OnDamageOther(FGameplayTag EventTag, const FGameplayEventData* Payload);
	
	UFUNCTION(Client, Reliable)
	void Client_SpawnDamageNumber(FGameplayEventData Payload);
	
	UPROPERTY()
	FTimerHandle AutoSaveHandle;
	
	UFUNCTION()
	void OnSaveLoaded(UBSSaveGame* SaveGame);
	
	void SetAutoSaveTimer();
	
	UFUNCTION()
	void OnAutoSave();
	
	UFUNCTION()
	void OnPlayerPaused();
	
	UFUNCTION()
	void OnPlayerResumed();
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_Initialized)
	bool bInitialized = false;
	
	UFUNCTION()
	void OnRep_Initialized() const;
	
	UPROPERTY(ReplicatedUsing=OnRep_PlayerColor)
	int32 PlayerColor = 0;
	
	UFUNCTION()
	void OnRep_PlayerColor() const;
	
	UPROPERTY(BlueprintAssignable)
	FBSPlayerStateDelegate OnInitComplete;
	
	UPROPERTY(BlueprintAssignable)
	FBSOnPlayerColorChanged OnPlayerColorChangedDelegate;
	
	UPROPERTY()
	TObjectPtr<UBSTalentComponent> TalentComponent;
	
	UPROPERTY()
	TObjectPtr<UBSAbilitySystemComponent> AbilitySystemComponent;
	
	UPROPERTY()
	TObjectPtr<UBSAttributeSet> AttributeSetSubObject;
};
