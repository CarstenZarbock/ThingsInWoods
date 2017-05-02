// Fill out your copyright notice in the Description page of Project Settings.

#include "ThingsInWoods.h"
#include "MotionScannerMobile.h"
#include "ThingsInWoodsCharacter.h"


AMotionScannerMobile::AMotionScannerMobile()
{
	
}

void AMotionScannerMobile::CustomTick()
{
	//Tick
	if (this->isScanning && GetGameTimeSinceCreation() > this->NextScanHitTime)
	{
		//Scan
		this->ActiveScanActors = this->Scan();
		this->DrawRadar();

		//Play Sounds
		if (this->ActiveScanActors.Num() > 0)
		{
			this->PlaySound(this->Beep, this->AudioComponent, false);
		}

		//---
		this->NextScanHitTime = GetGameTimeSinceCreation() + this->ScanTime;
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, "Scanning");
	}
}

void AMotionScannerMobile::DrawRadar()
{
	const FString command = TEXT("DrawRadarBP");
	FOutputDeviceDebug debug;
	this->CallFunctionByNameWithArguments(*command, debug, this, true);
}

TArray<AActor*> AMotionScannerMobile::Scan()
{
	//Scan
	//Get all characters
	TArray<AActor*> ActiveActors;

	for (TActorIterator<AThingsInWoodsCharacter> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		AThingsInWoodsCharacter* PlayerCharacter = *ActorItr;
		if (PlayerCharacter != nullptr && PlayerCharacter != this->GetOwner())
		{
			if (PlayerCharacter->GetDistanceTo(this) <= this->ScanDistance)
			{
				ActiveActors.Add(PlayerCharacter);
			}
		}
	}

	return ActiveActors;
}

void AMotionScannerMobile::execPrimary()
{
	if (this->isScanning)
	{
		this->DisableObject();
		return;
	}

	this->ActivateObject();
	return;	
}

void AMotionScannerMobile::execSecondary()
{

}

void AMotionScannerMobile::ActivateObject()
{
	//Enable Scanning
	this->isScanning = true;
	this->GetMesh()->SetMaterial(ScannerMaterialElementIndex, this->ScannerMaterialInstance);
	this->PlaySound(this->Toggle_On, this->AudioComponent, false);
}

void AMotionScannerMobile::DisableObject()
{

	//Disable Scanning
	this->isScanning = false;
	this->GetMesh()->SetMaterial(ScannerMaterialElementIndex, this->ScannerOfflineMaterial);
	this->PlaySound(this->Toggle_Off, this->AudioComponent, false);
}

void AMotionScannerMobile::BeginPlay()
{
	Super::BeginPlay();
	this->DisableObject();
	this->ScannerMaterialInstance = UMaterialInstanceDynamic::Create(this->ScannerMaterial, this);
}