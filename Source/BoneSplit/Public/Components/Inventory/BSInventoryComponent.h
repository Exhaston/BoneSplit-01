// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BSInventoryComponent.generated.h"

class UBSEquipmentEffect;
class ABSEquipmentDropBase;
class UAbilitySystemComponent;

//Struct to hold data on what loot has been instanced to clients and what is actually valid to give a client on request.
//Most of the data here is used by the server, but UI elements for the client might need them as well.
USTRUCT(BlueprintType)
struct FBSEquipmentDropInfo
{
	GENERATED_BODY()
	
	FBSEquipmentDropInfo() = default;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta=(IgnoreForMemberInitializationTest))
	FGuid LootGuid = FGuid::NewGuid();
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TSubclassOf<UBSEquipmentEffect> EquipmentEffect;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta=(ClampMin=0))
	int32 CurrentLevel = 0;
	
	UPROPERTY()
	FTransform EquipmentTransform = FTransform::Identity;
	
	bool operator==(const FBSEquipmentDropInfo& Other) const
	{
		return LootGuid == Other.LootGuid;
	}
};

DECLARE_MULTICAST_DELEGATE_OneParam(FBSOnPlayerColorChanged, FColor NewColor);
DECLARE_MULTICAST_DELEGATE_OneParam(FBSOnEquipmentChanged, const UBSEquipmentEffect* Equipment);

//Replicated component that lives on the Player State. 
//This logic could be directly on the player state, for clarity it's a subcomponent instead.
//This will handle equipment changes, inventory that can be saved and server authority loot.
UCLASS(NotBlueprintable, BlueprintType)
class BONESPLIT_API UBSInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	
	UBSInventoryComponent();
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual void BeginPlay() override;
	
	//Initialize for server and clients or right after asc construction
	void SetAbilitySystem(UAbilitySystemComponent* InAbilitySystemComponent);
	                      
	//Server RPC to equip item. If the equipment drop isn't spawned from the server, this won't work. 
	//Drop old will call SpawnEquipmentForPlayer() for old slot, don't enable this during initialization.
	UFUNCTION(Server, Reliable)
	void Server_EquipItem(const FBSEquipmentDropInfo& InDropInfo, bool bDropOld = false);
	
	//Spawns Equipment for the player. Only works for authority. Make sure Equipment is NOT replicated. 
	//It's per player instanced.  
	//The server then stores the spawn info for validation when client asks for equip.
	virtual void SpawnEquipmentForPlayer(
		TSubclassOf<UBSEquipmentEffect> EquipmentEffect, FTransform SpawnTransform, int32 InLevel = 0);
	
	//Returns delegate that triggers on Server and Client whenever equipment was applied.
	FBSOnEquipmentChanged& GetEquipmentEquippedDelegate() { return OnPlayerEquipmentEquipped; }
	                                             
	//Returns delegate that triggers on Server and Client whenever equipment was removed.
	FBSOnEquipmentChanged& GetEquipmentRemovedDelegate() { return OnPlayerEquipmentRemoved; }
	
	//returns the current equipment effects
	TArray<const UBSEquipmentEffect*> GetEquipment();
	
	//Sets the current color index to be that of a range valid in the project settings PlayerColors. 
	//Updates, replicates and triggers delegates.
	UFUNCTION(Server, Reliable)
	void Server_SetPlayerColor(int32 NewColor);
	
	//Returns the actual color from the current selected color index.
	FColor GetPlayerColor();
	
	FBSOnPlayerColorChanged& GetPlayerColorChangedDelegate() { return OnPlayerColorChangedDelegate; }

protected:
	
	virtual void CacheInitialEquipment();
	
	virtual void SendEquipmentCallback(const UBSEquipmentEffect* InNewEquipment);
	
	UPROPERTY()
	TArray<const UBSEquipmentEffect*> CachedCurrentEquipment;
	
	//Loops through the world to find existing Equipment drops,             
	//for the server to acknowledge valid interactions with them. Exec in normal branching, not as RPC.
	virtual void ValidateExistingWorldInstances();
	
	//Actually spawns the equipment on the owning client to interact with.
	UFUNCTION(Client, Reliable)                   
	void Client_SpawnEquipmentForPlayer(const FBSEquipmentDropInfo& InDropInfo);
	
	//Only valid on the server.
	UPROPERTY()
	TArray<FBSEquipmentDropInfo> EquipmentDropInfo;
	
	FBSOnEquipmentChanged OnPlayerEquipmentEquipped;
	FBSOnEquipmentChanged OnPlayerEquipmentRemoved;
	
	// -- Color --
	
	FBSOnPlayerColorChanged OnPlayerColorChangedDelegate;
	
	//A number determining what color is currently selected. The colors are found in developer settings.
	UPROPERTY(ReplicatedUsing=OnRep_PlayerColor)
	int32 CurrentColor = 0;
	
	UFUNCTION()
	void OnRep_PlayerColor();
	
	// -- Owner Utility --
	
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
	
	//Returns true if this is running on the server.
	bool OwnerHasAuthority();
	
	//Accessor for owner world, without needing to implement latent functionality to this component.
	UWorld* GetOwnerWorld();
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TSubclassOf<ABSEquipmentDropBase> EquipmentDropClass;
};

UINTERFACE(NotBlueprintable, NotBlueprintType)
class UBSInventoryInterface : public UInterface
{
	GENERATED_BODY()
};

class IBSInventoryInterface
{
	GENERATED_BODY()

public:
	                                                                          
	//Gets the current inventory component for the actor. 
	//Can be null on Player Character before PlayerState is Replicated / Setup.
	virtual UBSInventoryComponent* GetInventoryComponent() = 0;
	
};
