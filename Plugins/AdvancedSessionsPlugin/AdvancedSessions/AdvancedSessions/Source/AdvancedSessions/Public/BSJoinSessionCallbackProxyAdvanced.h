// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Net/OnlineBlueprintCallProxyBase.h"
#include "BSJoinSessionCallbackProxyAdvanced.generated.h"

struct FBlueprintSessionResult;

// Helper class for various methods to reduce the call hierarchy
struct FBSOnlineSubsystemBPCallHelper
{
	FBSOnlineSubsystemBPCallHelper(const TCHAR* CallFunctionContext, UObject* WorldContextObject, FName SystemName = NAME_None);

	void QueryIDFromPlayerController(APlayerController* PlayerController);

	bool IsValid() const
	{
		return UserID.IsValid() && (OnlineSub != nullptr);
	}
	
	FUniqueNetIdPtr UserID;
	IOnlineSubsystem* const OnlineSub;
	const TCHAR* FunctionContext;
};

/**
 * 
 */
UCLASS()
class ADVANCEDSESSIONS_API UBSJoinSessionCallbackProxyAdvanced : public UOnlineBlueprintCallProxyBase
{
	GENERATED_BODY()
	
public:

	UBSJoinSessionCallbackProxyAdvanced(const FObjectInitializer& ObjectInitializer);

// Called when there is a successful join
	
	FEmptyOnlineDelegate OnSuccess;

	// Called when there is an unsuccessful join
	UPROPERTY(BlueprintAssignable)
	FEmptyOnlineDelegate OnFailure;

	// Joins a remote session with the default online subsystem
	UFUNCTION(BlueprintCallable, meta=(BlueprintInternalUseOnly = "true", WorldContext="WorldContextObject"), Category = "Online|Session")
	static UBSJoinSessionCallbackProxyAdvanced* JoinSession(UObject* WorldContextObject, class APlayerController* PlayerController, const FBlueprintSessionResult& SearchResult);

	// UOnlineBlueprintCallProxyBase interface
	virtual void Activate() override;
	// End of UOnlineBlueprintCallProxyBase interface

private:
	// Internal callback when the join completes, calls out to the public success/failure callbacks
	void OnCompleted(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

private:
	// The player controller triggering things
	TWeakObjectPtr<APlayerController> PlayerControllerWeakPtr;

	// The search result we are sttempting to join
	FOnlineSessionSearchResult OnlineSearchResult;

	// The delegate executed by the online subsystem
	FOnJoinSessionCompleteDelegate Delegate;

	// Handle to the registered FOnJoinSessionComplete delegate
	FDelegateHandle DelegateHandle;

	// The world context object in which this call is taking place
	UPROPERTY()
	UObject* WorldContextObject;
};
