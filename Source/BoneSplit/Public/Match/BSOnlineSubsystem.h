// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "BSOnlineSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class BONESPLIT_API UBSOnlineSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	
	static UBSOnlineSubsystem* Get(const UObject* Context)
	{
		const UWorld* World = GEngine->GetWorldFromContextObjectChecked(Context);
		return World->GetGameInstance()->GetSubsystem<UBSOnlineSubsystem>();
	}
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	virtual void StartGame(bool bHosting);
	virtual void StartHosting();
	virtual void EndHosting();
	virtual void LeaveGame();
	
	virtual void SetEnableLan(const bool bEnable) { bLanMatch = bEnable; };
	
	virtual UWorld* GetWorld() const override;

protected:

	bool bIsHosting = false;
	bool bLanMatch = false;

	FSimpleMulticastDelegate OnGameLeftDelegate;
	FSimpleMulticastDelegate OnGameBeginDelegate;

private:

	IOnlineSessionPtr GetSessionInterface() const;
	
	// Delegate objects
	FDelegateHandle OnCreateSessionCompleteDelegateHandle;
	FDelegateHandle OnDestroySessionCompleteDelegateHandle;

	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
	void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);
	
};
