// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BaseItem.h"
#include "Flashlight.generated.h"

/**
*
*/
UCLASS()
class THINGSINWOODS_API AFlashlight : public ABaseItem
{
	GENERATED_BODY()

		AFlashlight();
	void BeginPlay() override;

public:
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		class USpotLightComponent* FlashLight;
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		class UStaticMeshComponent* VolumetricLight;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Materials)
		UMaterialInstanceDynamic* FlashlightMaterialInstance;
	//------------------------------------------------------------------------
	//Sounds
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Sounds)
		class USoundBase* Toggle;
	//------------------------------------------------------------------------
	bool isOn;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		float LightIntensity;

	void CustomTick() override;
	void execPrimary() override;
	void execSecondary() override;
	void ActivateObject() override;
	void DisableObject() override;
};