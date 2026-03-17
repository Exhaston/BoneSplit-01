// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "BSInteractionWorldRegistry.generated.h"


USTRUCT()
struct FClientInteractionInfo
{
	GENERATED_BODY()

	FGuid Guid;
};

UCLASS()
class BONESPLIT_API UBSInteractionWorldRegistry : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:
	
	FGuid RegisterInteractable(FGuid NewInteractableGuid);

	virtual void UnRegister(const FGuid& Guid);

	bool IsRegistered(const FGuid& Guid) const;

private:
	
	UPROPERTY()
	TArray<FGuid> RegisteredInteractionGuids;
};
