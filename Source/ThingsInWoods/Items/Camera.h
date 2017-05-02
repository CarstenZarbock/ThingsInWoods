// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BaseItem.h"
#include "Camera.generated.h"

/**
*
*/
UCLASS()
class THINGSINWOODS_API ACamera : public ABaseItem
{
	GENERATED_BODY()
		UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		class USceneCaptureComponent2D* SceneCapture;

	ACamera();
	void BeginPlay() override;

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Materials)
		UMaterialInstanceDynamic* CameraMaterialInstance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Materials)
		UMaterial* CameraMaterial;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		UTextureRenderTarget2D* RenderTarget;

	void execPrimary() override;
	void execSecondary() override;
	void ActivateObject() override;
	void DisableObject() override;

	void Register();
	void UnRegister();

	/** Returns SceneCapture subobject **/
	FORCEINLINE class USceneCaptureComponent2D* GetSceneCapture() const { return SceneCapture; }
};