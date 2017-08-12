// Fill out your copyright notice in the Description page of Project Settings.

#include "ThingsInWoods.h"
#include "MonitorLaser.h"
#include "MotionDetectorLaser.h"
#include "ThingsInWoodsCharacter.h"


AMonitorLaser::AMonitorLaser()
{
	this->isScanning = false;
}

void AMonitorLaser::CustomTick()
{
	//Tick
	if (this->isScanning)
	{
		
	}
}

void AMonitorLaser::DrawRadar()
{
	const FString command = TEXT("DrawRadarBP");
	FOutputDeviceDebug debug;
	this->CallFunctionByNameWithArguments(*command, debug, this, true);
}

void AMonitorLaser::execPrimary()
{
	AThingsInWoodsCharacter* PlayerPawn = Cast<AThingsInWoodsCharacter>(this->GetOwner());
	if (PlayerPawn != nullptr)
	{

	}
}

void AMonitorLaser::execSecondary()
{
	AThingsInWoodsCharacter* PlayerPawn = Cast<AThingsInWoodsCharacter>(this->GetOwner());
	if (PlayerPawn != nullptr)
	{

	}
}

void AMonitorLaser::ActivateObject()
{
	//Register on the computer objects
	this->isScanning = true;
}

void AMonitorLaser::DisableObject()
{
	//Reset
	this->isScanning = false;
}

void AMonitorLaser::BeginPlay()
{
	Super::BeginPlay();
	this->DisableObject();
	this->ScannerMaterialInstance = UMaterialInstanceDynamic::Create(this->ScannerMaterial, this);
}

void AMonitorLaser::RegisterScanner(ABaseItem* ScannerActor)
{
	for (int i = 0; i < this->ActiveScanners.Num(); i++)
	{
		if (this->ActiveScanners[i] == ScannerActor)
		{
			//Already registered, cancel
			return;
		}
	}

	this->ActiveScanners.Add(ScannerActor);
}

void AMonitorLaser::UnRegisterScanner(ABaseItem* ScannerActor)
{
	for (int i = 0; i < this->ActiveScanners.Num(); i++)
	{
		if (this->ActiveScanners[i] == ScannerActor)
		{
			this->ActiveScanners.RemoveAt(i);
			return;
		}
	}
}

void AMonitorLaser::UseObject()
{

}