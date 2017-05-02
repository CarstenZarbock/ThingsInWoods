// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BaseItem.h"
#include "Engine.h"
#include "MonitorMotionRadar.generated.h"

/**
*
*/
UCLASS()
class THINGSINWOODS_API AMonitorMotionRadar : public ABaseItem
{
	GENERATED_BODY()

		AMonitorMotionRadar();
	void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Misc)
	TArray<ABaseItem*> ActiveScanners;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Misc)
		TArray<AActor*> ActiveActors;

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
	void ActivateScanner(int index);
	void SwitchScanner();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Misc)
	int ActiveScannerIndex;

	void Scan();
	float NextScanTimer;
};