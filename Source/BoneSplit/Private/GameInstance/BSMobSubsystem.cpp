// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "GameInstance/BSMobSubsystem.h"

#include "AIController.h"
#include "NavigationSystem.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PawnMovementComponent.h"
#include "GameFramework/PlayerState.h"

FBSTargetGroup* UBSMobSubsystem::FindGroup(AActor* Player)
{
    for (FBSTargetGroup& Group : TargetGroups)
    {
        if (Group.Target == Player)
        {
            return &Group;
        }
    }
    return nullptr;
}

void UBSMobSubsystem::RegisterTarget(
    AAIController* AIController,
    AActor* Target)
{
    if (AIController || Target)
        return;

    FBSTargetGroup* Group = FindGroup(Target);

    if (!Group)
    {
        FBSTargetGroup NewGroup;
        NewGroup.Target = Target;
        TargetGroups.Add(NewGroup);
        Group = &TargetGroups.Last();
    }
    
    AIController->SetFocus(Target, EAIFocusPriority::Gameplay);
    
    Group->AIs.AddUnique(AIController);
    
    AIController->MoveToActor(
    Group->Target.Get(), 
    -1, 
    false, 
    true);
}

void UBSMobSubsystem::UnRegisterTarget(AAIController* AIController)
{
    if (!AIController) return;
    
    AIController->ClearFocus(EAIFocusPriority::Gameplay);
    
    for (int32 i = TargetGroups.Num() - 1; i >= 0; --i)
    {
        TargetGroups[i].AIs.Remove(AIController);

        if (TargetGroups[i].AIs.Num() == 0)
        {
            TargetGroups.RemoveAt(i);
        }
    }
}

AActor* UBSMobSubsystem::GetRandomPlayer()
{
    if (const AGameStateBase* GS = GetWorld()->GetGameState(); !GS->PlayerArray.IsEmpty())
    {
        return GS->PlayerArray[FMath::RandRange(0, GS->PlayerArray.Num() - 1)]->GetPawn();
    }
    return nullptr;
}

AActor* UBSMobSubsystem::GetPlayerDeterministic()
{
    if (const AGameStateBase* GS = GetWorld()->GetGameState(); !GS->PlayerArray.IsEmpty())
    {
        AActor* SelectedPawn = GS->PlayerArray[CurrentPlayerIncrement % GS->PlayerArray.Num()]->GetPawn();
        CurrentPlayerIncrement++;
        return SelectedPawn;
    }
    return nullptr;
}

void UBSMobSubsystem::Tick(const float DeltaTime)
{
}

TStatId UBSMobSubsystem::GetStatId() const
{
    RETURN_QUICK_DECLARE_CYCLE_STAT(UBSMobSubsystem, STATGROUP_Tickables) 
}
