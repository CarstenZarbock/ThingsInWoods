// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BaseItem.h"
#include "Engine.h"
#include "MonitorLaser.generated.h"

/**
*
*/
UCLASS()
class THINGSINWOODS_API AMonitorLaser : public ABaseItem
{
	GENERATED_BODY()

		AMonitorLaser();
	void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Misc)
		TArray<ABaseItem*> ActiveScanners;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Materials)
		UMaterial* ScannerOfflineMaterial;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Materials)
		UMaterialInstanceDynamic* ScannerMaterialInstance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Materials)
		UMaterial* ScannerMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Materials)
		UCanvasRenderTarget2D* ScannerCanvas;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Materials)
		UTexture2D* DotTexture;

	void DrawRadar();

	bool isScanning;
	void CustomTick() override;

	void execPrimary() override;
	void execSecondary() override;
	void ActivateObject() override;
	void DisableObject() override;
	void UseObject() override;

	void RegisterScanner(ABaseItem* ScannerActor);
	void UnRegisterScanner(ABaseItem* ScannerActor);

	float NextScanTimer;
};