// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "BSWServerBrowser.generated.h"

class UCommonLazyImage;
class UBSToggleButton;
struct FBlueprintSessionResult;
class UBSWButtonBase;
class UBSWServerButton;

/**
 * 
 */
UCLASS()
class BONESPLIT_API UBSWServerBrowser : public UCommonActivatableWidget
{
	GENERATED_BODY()
	
public:
	
	virtual void NativeConstruct() override;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TSubclassOf<UBSWServerButton> ServerButtonClass;
	
	virtual void NativeOnActivated() override;
	
	UPROPERTY(meta=(BindWidget))
	UBSWButtonBase* HostButton;
	UPROPERTY(meta=(BindWidget))
	UBSWButtonBase* RefreshButton;
	UPROPERTY(meta=(BindWidget))
	UBSToggleButton* SearchLanButton;
	UPROPERTY(meta=(BindWidget))
	UBSToggleButton* PrivateToggleButton;
	
	UPROPERTY()
	bool bEnableLan = false;
	
	UPROPERTY()
	bool bPrivateSession = false;
	
	UPROPERTY(meta=(BindWidget))
	UPanelWidget* SessionPanel;
	
	UPROPERTY(meta=(BindWidget))
	UCommonLazyImage* SearchingImage;
	
	UPROPERTY()
	TArray<FBlueprintSessionResult> FoundSessions;
	
	virtual void StartSearch();
	
	virtual void JoinSessionAt(int32 Index);
	
	UFUNCTION()
	void OnJoinFailed();
	
	UFUNCTION()
	void OnHostComplete();
	
	UFUNCTION()
	void OnSessionFindFailure(const TArray<FBlueprintSessionResult>& Sessions);
	
	UFUNCTION()
	void OnSessionsFound(const TArray<FBlueprintSessionResult>& Sessions);
};
