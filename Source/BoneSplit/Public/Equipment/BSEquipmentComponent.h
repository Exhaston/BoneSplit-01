// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BSEquipmentInstance.h"
#include "BSEquipmentComponent.generated.h"

struct FBSEquipmentMeshData;
class ABSEquipmentPickupActor;
class UAbilitySystemComponent;
class UBSEquipmentAsset;
struct FBSEquipmentInstance;

USTRUCT(Blueprintable, BlueprintType)
struct FBSEquipPickupInfo
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	FGuid EquipmentPickupInfo;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FBSEquipmentInstance EquipmentInstance;
	
	UPROPERTY()
	FVector WorldLocation = FVector::ZeroVector;
	
	bool operator==(const FBSEquipPickupInfo& Other) const
	{
		return EquipmentPickupInfo == Other.EquipmentPickupInfo;
	}
};
													 
//Local only struct to carry and tracking data for equipped equipment.
USTRUCT()
struct FBSActiveEquipmentData
{
	GENERATED_BODY()
	
	UPROPERTY()
	TArray<UBSEquipmentSkeletalMeshComponent*> MeshComps;
	
	UPROPERTY(Transient)
	TArray<FActiveGameplayEffectHandle> GrantedEffectHandles;
	
	UPROPERTY(Transient)
	TArray<FGameplayAbilitySpecHandle> GrantedSpecHandles;
};

DECLARE_MULTICAST_DELEGATE_OneParam(FBSOnEquipmentDelegate, const FBSEquipmentInstance& EquipmentInstance);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class BONESPLIT_API UBSEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	
	explicit UBSEquipmentComponent();

	static UBSEquipmentComponent* GetEquipmentComponent(const APlayerState* PS);
	
	virtual void BeginPlay() override;
	
	virtual void SetEquipmentOwner(ACharacter* InOwnerCharacter, UAbilitySystemComponent* InOwnerAbilitySystem);
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	//Applies the equipment from a world pickup. 
	//This assumes the server initially spawned the equipment and knows about it. If not this won't function
	UFUNCTION(Server, Reliable)
	void Server_RequestEquipFromDrop(const FBSEquipPickupInfo& Info);
	
	virtual void DropEquipmentForPlayer(const FBSEquipPickupInfo& DropInfo);
	
	virtual void ApplyEquipment(FBSEquipmentInstance NewEquipmentInstance, FBSEquipmentInstance& OldInstance);
	virtual void ApplyEquipmentByClass(const UBSEquipmentAsset* InEquipmentAsset);
	virtual void ForceRemoveEquipment(const UBSEquipmentAsset* InEquipmentAsset);
	virtual void ForceRemoveEquipment(const FBSEquipmentInstance& InEquipmentAsset);
	
	virtual void SetColorOnMeshes(FColor InEquipmentColor);
	
protected:
	
	virtual bool IsOwnerAuthority() const { return GetOwner() && GetOwner()->HasAuthority(); }
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TSubclassOf<ABSEquipmentPickupActor> EquipDropActorClass;
	
	//How far away the player can be from an equipment drop before the request is canceled
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float EquipmentServerMaxLeash = 500;
	
	UFUNCTION(Client, Reliable)
	void Client_SpawnEquipment(const FBSEquipPickupInfo& DropInfo);
	
	UPROPERTY(Transient)
	TArray<FBSEquipPickupInfo> ServerWorldDropInfos;
	
	UPROPERTY(Transient)
	TObjectPtr<UAbilitySystemComponent> OwnerAsc;
	
	UPROPERTY(Transient)
	TObjectPtr<ACharacter> OwnerCharacter;
	
	FBSOnEquipmentDelegate OnEquipmentApplied;
	FBSOnEquipmentDelegate OnEquipmentRemoved;
	
	UPROPERTY(ReplicatedUsing=OnRep_EquipmentInstances)
	TArray<FBSEquipmentInstance> EquipmentInstances;
	
	UFUNCTION()
	void OnRep_EquipmentInstances(TArray<FBSEquipmentInstance> OldInstances);
	
	virtual void Internal_ApplyEquipmentInstance(FBSEquipmentInstance EquipmentInstance);
	virtual void Internal_ClearEquipmentInstance(FBSEquipmentInstance EquipmentInstance);
	
	UPROPERTY()
	TMap<FBSEquipmentInstance, FBSActiveEquipmentData> MeshInstances;
};
