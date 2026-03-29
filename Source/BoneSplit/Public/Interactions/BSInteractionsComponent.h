// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BSInteractionsComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FBSOnInteractionDelegate, const FGuid&);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class BONESPLIT_API UBSInteractionsComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	
	explicit UBSInteractionsComponent(const FObjectInitializer& ObjectInitializer);

	void RegisterInteractable(FGuid NewInteractableGuid);
	
	virtual void RequestInteraction(const FGuid& InteractableGuid);
	
	UFUNCTION(Server, Reliable)
	void Server_RequestInteraction(const FGuid& Guid);
	
protected:
	
	UFUNCTION(Client, Reliable)
	void Client_AcceptInteraction(const FGuid& Guid);
	
	UFUNCTION(Client, Reliable)
	void Client_RejectInteraction(const FGuid& Guid);
	
	bool IsRegistered(const FGuid& Guid) const;
	
	void UnRegister(const FGuid& Guid);
	
	//Player specific Interactions
	UPROPERTY()
	TArray<FGuid> RegisteredInteractions;
	
	FBSOnInteractionDelegate OnInteractionAccepted;
	FBSOnInteractionDelegate OnInteractionRejected;
	
	virtual void BeginPlay() override;
};
