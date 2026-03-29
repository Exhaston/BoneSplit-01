// Copyright (c) 2025 Electric Jolt. All Rights Reserved.

#pragma once
													   
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BSSkyboxBase.generated.h"

//TODO make sure mobility settings are correct in regards to compatability with different lighting modes.
//An optional skybox base actor. Handy because 'optimal' shadow settings, light baking settings have been preset.
UCLASS(DisplayName="BS SkyBox Base", Category="BoneSplit")
class BONESPLIT_API ABSSkyboxBase : public AActor
{
	GENERATED_BODY()
																											   
public:
	
	explicit ABSSkyboxBase(const FObjectInitializer& ObjectInitializer);

protected:
	
	//Directional light component. Remember to set Active = false if unused.
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	UDirectionalLightComponent* DirectionalLightComponent;
	
	//Skylight component. Remember to set Active = false if unused.
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	USkyLightComponent* SkyLightComponent;
	
	//Sky Sphere Mesh. Set to inverted sphere and make sure it's big enough to cover the scene. 
	//Use a sky sphere material with a texture cube using inverse camera vector coordinates as UVs.
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	UStaticMeshComponent* SkyboxSphereMesh;
};
