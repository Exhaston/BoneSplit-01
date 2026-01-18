// Copyright (c) 2025 Electric Jolt. All Rights Reserved.


#include "Actors/Skybox/BSSkyboxBase.h"

#include "Components/DirectionalLightComponent.h"
#include "Components/SkyLightComponent.h"

ABSSkyboxBase::ABSSkyboxBase(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	SkyboxSphereMesh = CreateDefaultSubobject<UStaticMeshComponent>("SkyboxSphereMesh");
	SkyboxSphereMesh->SetMobility(EComponentMobility::Type::Stationary);
	SkyboxSphereMesh->bSelectable = false;
	SkyboxSphereMesh->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	SkyboxSphereMesh->SetGenerateOverlapEvents(false);
	SkyboxSphereMesh->bTreatAsBackgroundForOcclusion = true;
	SkyboxSphereMesh->bUseAsOccluder = false;
	SkyboxSphereMesh->bEmissiveLightSource = true;
	SkyboxSphereMesh->CastShadow = false;
	SkyboxSphereMesh->SetReceivesDecals(false);
	SetRootComponent(SkyboxSphereMesh);
	
	SkyLightComponent = CreateDefaultSubobject<USkyLightComponent>(TEXT("SkyLightComponent"));
	SkyLightComponent->SetupAttachment(RootComponent);
	SkyLightComponent->bRealTimeCapture = false;
	SkyLightComponent->bLowerHemisphereIsBlack = false;
	SkyLightComponent->SetMobility(EComponentMobility::Type::Stationary);
	
	DirectionalLightComponent = CreateDefaultSubobject<UDirectionalLightComponent>(TEXT("DirectionalLight"));
	DirectionalLightComponent->SetMobility(EComponentMobility::Type::Stationary);
	DirectionalLightComponent->SetupAttachment(RootComponent);
	DirectionalLightComponent->Intensity = 1;
	DirectionalLightComponent->SetRelativeRotation({-45, 0, 0});
	DirectionalLightComponent->ShadowResolutionScale = 8;
	DirectionalLightComponent->LightSourceAngle = 0;
	DirectionalLightComponent->LightSourceSoftAngle = 0;
	DirectionalLightComponent->LightmassSettings.LightSourceAngle = 0;
	DirectionalLightComponent->ShadowSharpen = 0.5;
	DirectionalLightComponent->DynamicShadowDistanceStationaryLight = 4096;
	DirectionalLightComponent->DynamicShadowDistanceMovableLight = 4096;
	DirectionalLightComponent->bAtmosphereSunLight = true;
}

