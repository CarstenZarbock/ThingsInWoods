// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BaseItem.h"
#include "DSLRCamera.generated.h"

/**
*
*/
UCLASS()
class THINGSINWOODS_API ADSLRCamera : public ABaseItem
{
	GENERATED_BODY()

		ADSLRCamera();
	void BeginPlay() override;

public:
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		class USpotLightComponent* FlashLight;

	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		class USceneCaptureComponent2D* SceneCapture;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		UTextureRenderTarget2D* RenderTarget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Materials)
		UMaterialInstanceDynamic* CameraMaterialInstance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Materials)
		UMaterial* CameraMaterial;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		int DisplayMaterialElementIndex;

	//------------------------------------------------------------------------
	//Sounds
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Sounds)
		class USoundBase* Toggle_On;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Sounds)
		class USoundBase* Toggle_Off;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Sounds)
		class USoundBase* Flash_Loading;
	//------------------------------------------------------------------------


	bool hasFlashed;

	float ReloadTimer;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		float ReloadTime;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		float FlashTime;
	float FlashTimer;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		float FlashIntensity;

	void HandleTargets();

	void CustomTick() override;
	void execPrimary() override;
	void execSecondary() override;
	void ActivateObject() override;
	void DisableObject() override;
};