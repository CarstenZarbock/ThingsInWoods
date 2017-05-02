// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BaseItem.h"
#include "MonitorCamera.generated.h"

/**
*
*/
UCLASS()
class THINGSINWOODS_API AMonitorCamera : public ABaseItem
{
	GENERATED_BODY()

	AMonitorCamera();
	void BeginPlay() override;

public:
	TArray<ABaseItem*> ActiveCameras;
	UTexture2D* CurrentCamTexture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Materials)
		UMaterial* CameraOfflineMaterial;

	void execPrimary() override;
	void execSecondary() override;
	void ActivateObject() override;
	void DisableObject() override;
	void UseObject() override;

	void RegisterCamera(ABaseItem* CameraActor);
	void UnRegisterCamera(ABaseItem* CameraActor);
	void ActivateCamera(int index);
	void SwitchCamera();

	int ActiveCameraIndex;
};