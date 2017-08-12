// Fill out your copyright notice in the Description page of Project Settings.

#include "ThingsInWoods.h"
#include "MotionRadar.h"
#include "MonitorMotionRadar.h"
#include "ThingsInWoodsCharacter.h"


AMotionRadar::AMotionRadar()
{

}

void AMotionRadar::CustomTick()
{
	//Tick
	if (this->isScanning)
	{
		//Scan
		//this->Scan();
	}
}

TArray<AActor*> AMotionRadar::Scan()
{
	//Scan
	//Get all characters
	TArray<AActor*> ActiveActors;

	for (TActorIterator<AThingsInWoodsCharacter> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		AThingsInWoodsCharacter* PlayerCharacter = *ActorItr;
		if (PlayerCharacter != nullptr)
		{
			if (PlayerCharacter->GetDistanceTo(this) <= this->ScanDistance)
			{
				ActiveActors.Add(PlayerCharacter);
			}
		}
	}

	return ActiveActors;
}

void AMotionRadar::execPrimary()
{
	AThingsInWoodsCharacter* PlayerPawn = Cast<AThingsInWoodsCharacter>(this->GetOwner());
	if (PlayerPawn != nullptr)
	{
		// drop
		PlayerPawn->PlantObject();
	}
}

void AMotionRadar::execSecondary()
{
	AThingsInWoodsCharacter* PlayerPawn = Cast<AThingsInWoodsCharacter>(this->GetOwner());
	if (PlayerPawn != nullptr)
	{

	}
}

void AMotionRadar::ActivateObject()
{
	//Register on the computer objects
	this->Register();

	//Enable Scanning
	this->isScanning = true;
}

void AMotionRadar::DisableObject()
{
	//Unregister on the computer objects
	this->UnRegister();

	//Disable Scanning
	this->isScanning = false;
}

void AMotionRadar::BeginPlay()
{
	Super::BeginPlay();
	this->DisableObject();
}

void AMotionRadar::Register()
{
	//Get all characters
	for (TActorIterator<AMonitorMotionRadar> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		AMonitorMotionRadar* MonitorActor = *ActorItr;
		if (MonitorActor != nullptr)
		{
			MonitorActor->RegisterScanner(this);
		}
	}
}

void AMotionRadar::UnRegister()
{
	for (TActorIterator<AMonitorMotionRadar> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		AMonitorMotionRadar* MonitorActor = *ActorItr;
		if (MonitorActor != nullptr)
		{
			MonitorActor->UnRegisterScanner(this);
		}
	}
}