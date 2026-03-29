// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Threat/MThreatComponent.h"
#include "Threat/MThreatInterface.h"
#include "MMobController.generated.h"

class AMMobPatrolPath;
class UMMobPatrolComponent;

namespace MobConsoleVariables
{
	extern TAutoConsoleVariable<bool> CVarMMobVicinityDebug;
	extern TAutoConsoleVariable<bool> CVarMMobFocusDebug;
	extern TAutoConsoleVariable<bool> CVarMMobThreatDebug;
	extern TAutoConsoleVariable<bool> CVarMMobAlliesDebug;
}

DECLARE_MULTICAST_DELEGATE_OneParam(FMOnMobTick, float DeltaSeconds);
DECLARE_MULTICAST_DELEGATE_OneParam(FMOnFocusTargetChanged, AActor* NewFocus);

UCLASS(DisplayName="Mob Controller", BlueprintType, Blueprintable)
class MOBS_API AMMobController : public AAIController, public IMThreatInterface
{
	GENERATED_BODY()

public:
	
	explicit AMMobController(const FObjectInitializer& ObjectInitializer);
	
	virtual void Tick(float DeltaSeconds) override;
	
#if UE_BUILD_DEVELOPMENT || WITH_EDITOR
	
	virtual void DebugTargets();
	
#endif
	
	/* Public Setters */
	
	virtual void StartPatrol(AMMobPatrolPath* InPatrolPath);
	
	virtual bool LineOfSightTo(const AActor* Other, FVector ViewPoint = FVector(ForceInit), bool bAlternateChecks = false) const override;
	
	//When the mob should face something or inherently target something (like an ability) this should be set. 
	//Remember to ClearFocus() after the ability or event is over.
	virtual void SetCurrentTarget(AActor* InFocus);
	
	//Clears the current mob focus.
	virtual void ClearCurrentTarget();
	
	//Forces the movement to pause until resumed.
	virtual void SetMovementPaused(bool bNewMovementPaused);
	
	//Sets the new distance to hold from the current target, will refresh on next move request during mob tick.
	virtual void SetMoveAcceptanceRadius(const float InNewAcceptanceRadius)
	{
		TargetAcceptanceRadius = FMath::Clamp(InNewAcceptanceRadius, 0.f, FLT_MAX);
	}
	
	/* Public Getters */
	
	//Highest threat target that typically is the move target, may often be nullptr.
	virtual AActor* GetHighestThreatActor();
	
	//Current Target being targeted by an ability or anything requiring directionality, may often be nullptr.
	virtual AActor* GetCurrentTargetActor() { return GetFocusActorForPriority(EAIFocusPriority::Gameplay); }
	      
	//Gets all the allies gathered within the pawns vicinity and in line of sight.
	virtual TArray<AActor*> GetAlliesInVicinity();
	
	bool GetIsMoving();

	virtual FMOnTargetChanged& GetOnTargetChangedDelegate() { return OnTargetChangedDelegate; }
	
	virtual FMOnMobTick& GetOnMobTickDelegate() { return OnMobTickDelegate; }
	
	UMMobPatrolComponent* GetMobPatrolComponent() { return PatrolComponent; }
	
	virtual UMThreatComponent* GetThreatComponent() override { return ThreatComponent; }
	
	virtual FMOnFocusTargetChanged& GetOnFocusTargetChangedDelegate() { return OnFocusTargetChanged; }

protected:
	
	virtual void SetFocus(AActor* NewFocus, EAIFocusPriority::Type InPriority = EAIFocusPriority::Gameplay) override;
	virtual void ClearFocus(EAIFocusPriority::Type InPriority) override;
	
	FMOnFocusTargetChanged OnFocusTargetChanged;
	
	/* Mob tick without using a subcomponent */
	
	FMOnMobTick OnMobTickDelegate;
	
	//How often the mob should tick more expensive functions like vicinity gathering and move requests.
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta=(ClampMin=0, Units=Seconds), Category="Mob|Tick")
	float MobTickRate = 0.5f;
	
	UPROPERTY()
	float CurrentMobTickTime = 0;
	
	//Returns true if it's time for the mob to tick again, and if it's safe to do so.
	virtual bool AdvanceMobTick(float DeltaSeconds);
	
	/* Movement */
	
	//The distance the mob will keep from the current highest threat target.
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta=(ClampMin=0, Units=Centimeters), Category="Mob|Movement")
	float TargetAcceptanceRadius = 250;
	
	//Useful for when an AI should target something else than it's current chase target. 
	//Abilities should set focus for this controller and clear it after.
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Mob|Movement")
	bool bAutoPauseMovementWhenFocusSet = true;
	
	UPROPERTY()
	bool bMovementIssuingPaused = false;
	
	//Attempts to request new movement toward the current target.
	virtual void TryMoveToTarget(float DeltaSeconds);
	
	//Projects the vector input to the navigation floor. if it fails will return the original position.
	virtual void ProjectLocationToNavigation(FVector& Location, FVector Extents = {0,0,500});
	
	//When the threat component has a new highest threat 
	virtual void OnTargetChanged(AActor* OldActor, AActor* NewActor);
	
	virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;
	
	//Delegate that's called whenever the pawn moved to a location successfully.
	FSimpleMulticastDelegate OnMovementSuccessDelegate;
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category="Mob|Threat")
	TObjectPtr<UMMobPatrolComponent> PatrolComponent;
	          
	/* Threat */
	
	FMOnTargetChanged OnTargetChangedDelegate;
	                                                                 
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category="Mob|Threat")
	TObjectPtr<UMThreatComponent> ThreatComponent;
	
	UPROPERTY(Transient)                                                  
	TWeakObjectPtr<AActor> CurrentTarget = nullptr;
	
	/* Vicinity gathering allies and enemies */
	
	UPROPERTY()
	TArray<TWeakObjectPtr<AActor>> NearbyAllies;
	
	//Finds all the pawns in vicinity, radius defined by VicinitySphereRadius.
	//Filtering is done by overriding CheckActorEnemy() and CheckActorAlly().
	virtual void FindPawnsInVicinity(float DeltaSeconds);
	
	//Apply custom logic to see if the actor is an enemy. By default compares if no matching actor tags are found.
	virtual bool IsActorEnemy(AActor* InActor);
	
	//Apply custom logic to see if the actor is an ally. Uses matching actor tags by default.
	virtual bool IsActorAlly(AActor* InActor);
	
	//Distance in centimeters from the pawns origin. This is the sphere that will gather enemies and allies.
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta=(ClampMin=0, Units=Centimeters), Category="Mob|Vicinity")
	float VicinitySphereRadius = 2000;
	                                                                             
	//When true, pawns gathered within vicinity will be tested for visibility before going further
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Mob|Vicinity")
	bool bRequireLineOfSightForPawnGathering = true;

	//Only add threat to enemies in vicinity once. 
	//If false it will keep adding threat every mob tick, for every enemy within the vicinity radius.
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Mob|Vicinity")
	bool bAddThreatFromVicinityOnce = true;
};
