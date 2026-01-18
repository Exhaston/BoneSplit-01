// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "BSTargetSetting.generated.h"

UENUM(BlueprintType, DisplayName="Target Test Filter", Category="BoneSplit")
enum EBSTargetTestFilter : uint8
{
	Friendly       = 0,
	Enemy          = 1,
	Self           = 2,
	TestReachable  = 3,
	TestVisibility = 4
};                                                          

UENUM(BlueprintType, DisplayName="Direction Check", Category="BoneSplit")
enum EBSDirectionCheck : uint8
{
	Ebs_Forward UMETA(DisplayName="Forward"),
	Ebs_Behind UMETA(DisplayName="Behind"),
};

/*
 * Generic Target filter that is intended to be used with blueprints. 
 * Most expensive check is reachable, as it creates a path with the Nav System synchronously.
 * Generally test for reachable when picking a target, when applying damage check faction and visibility if needed.
 */
USTRUCT(BlueprintType, DisplayName="Target Filter", Category="BoneSplit")
struct FBSTargetFilter
{
	GENERATED_BODY()
	
	FBSTargetFilter() = default;
	                                                             
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Targeting")
	TSet<TEnumAsByte<EBSTargetTestFilter>> Tests = { Enemy };
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Targeting")
	TOptional<TEnumAsByte<EBSDirectionCheck>> DirectionCheck;
	
	bool RunFilter(AActor* AvatarActor, AActor* TestActor);
	
	//Returns true if the target is reachable by AI pathfinding
	static bool TestReachable(AActor* AvatarActor, const AActor* TargetActor);          
	
	//Returns true if there is no obstruction of vision
	static bool TestVisibility(AActor* AvatarActor, const AActor* TargetActor);
	
	//Returns true if any factions are shared between the two actors
	static bool TestFaction(AActor* AvatarActor, AActor* TargetActor);
};

//TODO include tests for dead.
/**
 * Base for creating new target modes for Mob logic.
 */
UCLASS(DefaultToInstanced, EditInlineNew, Abstract, CollapseCategories, Category="BoneSplit", ClassGroup="BoneSplit")
class BONESPLIT_API UBSTargetSetting : public UObject
{
	GENERATED_BODY()
	
public:
	
	virtual AActor* GetTarget(AActor* AvatarActor) { return nullptr; }
	
	FBSTargetFilter GetTargetFilter() { return TargetFilter; }
	
protected:
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FBSTargetFilter TargetFilter;
};

UCLASS(Abstract)
class UBSTargetSetting_RangeCheckBase : public UBSTargetSetting
{
	GENERATED_BODY()

public:          
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Targeting")
	float Range = 1000;
};

/**
 * Gets a random target in radius that matches the filters
 */
UCLASS(DisplayName="Radius->Random")
class UBSTargetSetting_RandomInRange: public UBSTargetSetting_RangeCheckBase
{
	GENERATED_BODY()

public: 
	
	virtual AActor* GetTarget(AActor* AvatarActor) override;
};

/**
 * Gets the closest target in radius that matches the filters
 */
UCLASS(DisplayName="Radius->Distance->Closest")
class UBSTargetSetting_Closest: public UBSTargetSetting_RangeCheckBase
{
	GENERATED_BODY()

public:      
	
	virtual AActor* GetTarget(AActor* AvatarActor) override;
	
};

/**
 * Gets the farthest target in radius that matches the filters
 */
UCLASS(DisplayName="Radius->Distance->Farthest")
class UBSTargetSetting_Farthest: public UBSTargetSetting_RangeCheckBase
{
	GENERATED_BODY()

public:          
	virtual AActor* GetTarget(AActor* AvatarActor) override;
	
};

/**
 * Gets a random target from the threat list that matches the filters
 */
UCLASS(DisplayName="Threat->Random")
class UBSTargetSetting_RandomThreat: public UBSTargetSetting
{
	GENERATED_BODY()

public:    
	virtual AActor* GetTarget(AActor* AvatarActor) override;
	
};

/**
 * Gets the lowest possible target from threat list.
 */
UCLASS(DisplayName="Threat->Lowest")
class UBSTargetSetting_LowestThreat: public UBSTargetSetting
{
	GENERATED_BODY()

public:          
	virtual AActor* GetTarget(AActor* AvatarActor) override;
};

/**
 * Gets the highest possible target from threat list.
 */
UCLASS(DisplayName="Threat->Highest")
class UBSTargetSetting_HighestThreat: public UBSTargetSetting
{
	GENERATED_BODY()

public:         
	
	virtual AActor* GetTarget(AActor* AvatarActor) override;
	
};  

/**
 * Gets the closest possible target from threat list.
 */
UCLASS(DisplayName="Threat->Distance->Closest")
class UBSTargetSetting_ClosestThreat: public UBSTargetSetting
{
	GENERATED_BODY()

public:        
	
	virtual AActor* GetTarget(AActor* AvatarActor) override;
	
};  

/**
 * Gets the farthest possible target from threat list.
 */
UCLASS(DisplayName="Threat->Distance->Farthest")
class UBSTargetSetting_FarthestThreat: public UBSTargetSetting
{
	GENERATED_BODY()

public:       
	
	virtual AActor* GetTarget(AActor* AvatarActor) override;
	
}; 

/**
 * Gets the closest possible target from threat list.
 */
UCLASS(DisplayName="Threat->Health->Lowest")
class UBSTargetSetting_LowestHealthThreat: public UBSTargetSetting
{
	GENERATED_BODY()
										  
public:       
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta=(Units="%", ClampMin=0, ClampMax=100))
	float MaxHealthPercent = 50;
	
	virtual AActor* GetTarget(AActor* AvatarActor) override;
	
};                                              

UCLASS(DisplayName="Threat->Health->Highest")
class UBSTargetSetting_HighestHealthThreat: public UBSTargetSetting
{
	GENERATED_BODY()
										  
public:       
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta=(Units="%", ClampMin=0, ClampMax=100))
	float MinHealthPercent = 50;
	
	virtual AActor* GetTarget(AActor* AvatarActor) override;
	
}; 

/**
 * Gets the closest possible target from threat list.
 */
UCLASS(DisplayName="Radius->Health->Lowest")
class UBSTargetSetting_LowestHealthRadius: public UBSTargetSetting_RangeCheckBase
{
	GENERATED_BODY()
										  
public:       
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta=(Units="%", ClampMin=0, ClampMax=100))
	float MaxHealthPercent = 50;
	
	virtual AActor* GetTarget(AActor* AvatarActor) override;
	
};                                              

UCLASS(DisplayName="Radius->Health->Highest")
class UBSTargetSetting_HighestHealthRadius: public UBSTargetSetting_RangeCheckBase
{
	GENERATED_BODY()
										  
public:    
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta=(Units="%", ClampMin=0, ClampMax=100))
	float MinHealthPercent = 50;
	
	virtual AActor* GetTarget(AActor* AvatarActor) override;
	
}; 
