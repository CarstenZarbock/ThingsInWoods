// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BaseItem.h"
#include "Engine.h"
#include "MotionScannerMobile.generated.h"

/**
*
*/
UCLASS()
class THINGSINWOODS_API AMotionScannerMobile : public ABaseItem
{
	GENERATED_BODY()

		AMotionScannerMobile();
	void BeginPlay() override;

public:
	//------------------------------------------------------------------------
	//Sounds
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Sounds)
		class USoundBase* Toggle_On;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Sounds)
		class USoundBase* Toggle_Off;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Sounds)
		class USoundBase* Beep;
	//------------------------------------------------------------------------

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Materials)
		UMaterial* ScannerOfflineMaterial;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Materials)
		UMaterialInstanceDynamic* ScannerMaterialInstance;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Materials)
		UMaterial* ScannerMaterial;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Materials)
		UCanvasRenderTarget2D* ScannerCanvas;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Materials)
		UTexture2D* DotTexture;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		int ScannerMaterialElementIndex;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		float ScanTime;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		float NextScanHitTime;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		float ScanDistance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	bool isScanning;
	TArray<AActor*> Scan();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category=Misc)
	TArray<AActor*> ActiveScanActors;

	void DrawRadar();
	void CustomTick() override;
	void execPrimary() override;
	void execSecondary() override;
	void ActivateObject() override;
	void DisableObject() override;
};