// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BaseItem.h"
#include "MotionRadar.generated.h"

/**
*
*/
UCLASS()
class THINGSINWOODS_API AMotionRadar : public ABaseItem
{
	GENERATED_BODY()

	AMotionRadar();
	void BeginPlay() override;

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		float ScanTime;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		float ScanDistance;

	bool isScanning;
	TArray<AActor*> Scan();

	void CustomTick() override;
	void execPrimary() override;
	void execSecondary() override;
	void ActivateObject() override;
	void DisableObject() override;

	void Register();
	void UnRegister();
};