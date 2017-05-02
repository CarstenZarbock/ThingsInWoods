// Fill out your copyright notice in the Description page of Project Settings.

#include "ThingsInWoods.h"
#include "MonitorMotionRadar.h"
#include "MotionRadar.h"
#include "ThingsInWoodsCharacter.h"


AMonitorMotionRadar::AMonitorMotionRadar()
{
	this->isScanning = false;
}

void AMonitorMotionRadar::CustomTick()
{
	//Tick
	if (this->isScanning)
	{
		this->Scan();
	}
}

void AMonitorMotionRadar::Scan()
{
	if (GetGameTimeSinceCreation() > this->NextScanTimer)
	{
		if (this->ActiveScanners[this->ActiveScannerIndex]->IsValidLowLevel())
		{
			AMotionRadar* CurrentScanner = Cast<AMotionRadar>(this->ActiveScanners[this->ActiveScannerIndex]);
			if (CurrentScanner != nullptr)
			{
				ActiveActors = CurrentScanner->Scan();

				//Send To ScanMonitor
				for (int i = 0; i < ActiveActors.Num(); i++)
				{
					FString Message;
					Message.Append("Scan Found: ");
					Message.Append(ActiveActors[i]->GetName());
					GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Green, Message);
				}

				this->NextScanTimer = GetGameTimeSinceCreation() + CurrentScanner->ScanTime;
				this->DrawRadar();
			}
		}
	}
}

void AMonitorMotionRadar::DrawRadar()
{
	const FString command = TEXT("DrawRadarBP");
	FOutputDeviceDebug debug;
	this->CallFunctionByNameWithArguments(*command, debug, this, true);
}

void AMonitorMotionRadar::execPrimary()
{
	AThingsInWoodsCharacter* PlayerPawn = Cast<AThingsInWoodsCharacter>(this->GetOwner());
	if (PlayerPawn != nullptr)
	{
		// drop
		PlayerPawn->PlantObject();
	}
}

void AMonitorMotionRadar::execSecondary()
{
	AThingsInWoodsCharacter* PlayerPawn = Cast<AThingsInWoodsCharacter>(this->GetOwner());
	if (PlayerPawn != nullptr)
	{

	}
}

void AMonitorMotionRadar::ActivateObject()
{
	//Register on the computer objects
	this->isScanning = true;
}

void AMonitorMotionRadar::DisableObject()
{
	//Reset
	this->isScanning = false;
	this->ActiveScannerIndex = -1;
	this->SwitchScanner();
}

void AMonitorMotionRadar::BeginPlay()
{
	Super::BeginPlay();
	this->DisableObject();
	this->ScannerMaterialInstance = UMaterialInstanceDynamic::Create(this->ScannerMaterial, this);
}

void AMonitorMotionRadar::RegisterScanner(ABaseItem* ScannerActor)
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
	this->ActivateScanner(this->ActiveScanners.Num() - 1);
}

void AMonitorMotionRadar::UnRegisterScanner(ABaseItem* ScannerActor)
{
	for (int i = 0; i < this->ActiveScanners.Num(); i++)
	{
		if (this->ActiveScanners[i] == ScannerActor)
		{
			this->ActiveScanners.RemoveAt(i);
			this->SwitchScanner();
			return;
		}
	}
}

void AMonitorMotionRadar::ActivateScanner(int index)
{
	if (index < this->ActiveScanners.Num())
	{
		if (this->ActiveScanners[index]->IsValidLowLevel())
		{
			AMotionRadar* ActiveScanner = Cast<AMotionRadar>(this->ActiveScanners[index]);
			if (ActiveScanner != nullptr)
			{
				this->GetMesh()->SetMaterial(0, this->ScannerMaterialInstance);
				this->ActiveScannerIndex = index;
				this->ActivateObject();
			}
			else
			{
				return;
			}
		}
	}
}

void AMonitorMotionRadar::SwitchScanner()
{
	if (this->ActiveScanners.Num() == 0)
	{
		//No Cameras, set offline Material
		this->GetMesh()->SetMaterial(0, this->ScannerOfflineMaterial);
		this->isScanning = false;
	}
	else
	{
		this->ActiveScannerIndex += 1;
		if (this->ActiveScannerIndex >= this->ActiveScanners.Num())
		{
			this->ActiveScannerIndex = 0;
		}

		this->ActivateScanner(this->ActiveScannerIndex);
	}
}

void AMonitorMotionRadar::UseObject()
{
	this->SwitchScanner();
}