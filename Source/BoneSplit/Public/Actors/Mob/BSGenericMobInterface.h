// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "BSGenericMobInterface.generated.h"

class UBSThreatComponent;

UINTERFACE(Blueprintable, BlueprintType, DisplayName="GenericMobInterface")
class UBSGenericMobInterface : public UInterface
{
	GENERATED_BODY()
};


class BONESPLIT_API IBSGenericMobInterface
{
	GENERATED_BODY()

public:
	
	//TODO: Add pure virtual native functions to avoid parent calls being necessary from BP.
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, DisplayName="GetAggroRange", Category="BoneSplit|MobInterface")
	float BP_GetAggroRange();
	
	virtual float BP_GetAggroRange_Implementation() { return 1000; }
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, DisplayName="IsInCombat", Category="BoneSplit|MobInterface")
	bool BP_IsInCombat();
	
	virtual bool BP_IsInCombat_Implementation() { return false; }
	
	virtual void NativeOnCombatTick(bool bReceivedToken, float DeltaTime) = 0;
	
	UFUNCTION(BlueprintNativeEvent, DisplayName="OnCombatTick", Category="BoneSplit|MobInterface")
	void BP_OnCombatTick(bool bReceivedToken, float DeltaTime);
	
	virtual void BP_OnCombatTick_Implementation(bool bReceivedToken, float DeltaTime) { }
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, DisplayName="IsReadyForToken", Category="BoneSplit|MobInterface")
	bool BP_IsReadyForToken();
	
	virtual bool BP_IsReadyForToken_Implementation() { return false; }
	
	virtual void NativeOnCombatBegin() = 0;
	
	UFUNCTION(BlueprintNativeEvent, DisplayName="OnCombatBegin", Category="BoneSplit|MobInterface")
	void BP_OnCombatBegin();
	
	virtual void BP_OnCombatBegin_Implementation() {}
	
	virtual void NativeOnCombatEnd() = 0;
	
	UFUNCTION(BlueprintNativeEvent, DisplayName="OnCombatEnd", Category="BoneSplit|MobInterface")
	void BP_OnCombatEnd();
	
	virtual void BP_OnCombatEnd_Implementation() {}
};
