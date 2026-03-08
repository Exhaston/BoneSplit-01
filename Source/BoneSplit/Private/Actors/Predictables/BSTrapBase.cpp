// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Actors/Predictables/BSTrapBase.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"


ABSTrapBase::ABSTrapBase()
{
	PrimaryActorTick.bCanEverTick = true;
	SetRootComponent(CreateDefaultSubobject<USceneComponent>("RootComponent"));
	TrapMesh = CreateDefaultSubobject<UStaticMeshComponent>("TrapMesh");
	TrapMesh->SetupAttachment(RootComponent);
	
	bReplicates = true;
	bAlwaysRelevant = true;
	SetNetUpdateFrequency(10);
	
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
}

void ABSTrapBase::BeginPlay()
{
	Super::BeginPlay();
	
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
}

void ABSTrapBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	TrapMesh->SetScalarParameterValueOnMaterials(MaterialParamName, 0);
	
	const ABSTrapBase* CDO = GetDefault<ABSTrapBase>(GetClass());

	if (const FRichCurve* RichCurve = CDO->AnimationCurve.GetRichCurveConst(); RichCurve && RichCurve->GetNumKeys() > 0)
	{
		UCurveFloat* RuntimeCurve = NewObject<UCurveFloat>(this);
		RuntimeCurve->FloatCurve = *RichCurve;
		
		OnTimelineFinishedDelegate.BindUFunction(this, "OnTimelineFinished");
		AnimTimeLine.SetTimelineFinishedFunc(OnTimelineFinishedDelegate);

		AnimProgressDelegate.BindUFunction(this, FName("HandleAnimProgress"));
		AnimTimeLine.AddInterpFloat(RuntimeCurve, AnimProgressDelegate);
		AnimTimeLine.SetPlayRate(1.f / AnimationSpeed);
	}
}

UAbilitySystemComponent* ABSTrapBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}


void ABSTrapBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	AnimTimeLine.TickTimeline(DeltaTime);
}

void ABSTrapBase::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	
	if (!HasAuthority() || !GetWorld()) return;
	
	if (!bReady) return;
	if (const IAbilitySystemInterface* AscI = Cast<IAbilitySystemInterface>(OtherActor))
	{
		 if (AscI->GetAbilitySystemComponent() && AscI->GetAbilitySystemComponent()->GetAvatarActor())
		 {
		 	PlayAnim();
		 	FGameplayEffectContextHandle ContextHandle = AbilitySystemComponent->MakeEffectContext();
		 	FVector OriginLoc = TrapMesh->GetUpVector() * -10 + AscI->GetAbilitySystemComponent()->GetAvatarActor()->GetActorLocation();
		 	ContextHandle.AddOrigin(OriginLoc);
		 	AbilitySystemComponent->BP_ApplyGameplayEffectToTarget(EffectToApply, AscI->GetAbilitySystemComponent(), 1, ContextHandle);
		 }
	}
}

void ABSTrapBase::PlayAnim_Implementation()
{
	if (!GetWorld()) return;
	TriggerTrap();
}

void ABSTrapBase::HandleAnimProgress(float Value)
{
	if (!GetWorld()) return;
	TrapMesh->SetScalarParameterValueOnMaterials(MaterialParamName, Value);
}

void ABSTrapBase::OnTimeLineFinished()
{
	
	ResetTrap();
}

void ABSTrapBase::TriggerTrap()
{
	bReady = false;
	AnimTimeLine.PlayFromStart();
	
	/*
	FTimerHandle CooldownHandle;
	GetWorldTimerManager().SetTimer(
		CooldownHandle,
		this,
		&ABSTrapBase::ResetTrap,
		Cooldown,
		false
	);
	*/
}

void ABSTrapBase::ResetTrap()
{
	bReady = true;
}

