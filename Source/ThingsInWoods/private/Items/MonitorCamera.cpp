// Fill out your copyright notice in the Description page of Project Settings.

#include "ThingsInWoods.h"
#include "MonitorCamera.h"
#include "Camera.h"
#include "ThingsInWoodsCharacter.h"


AMonitorCamera::AMonitorCamera()
{

}

void AMonitorCamera::execPrimary()
{
	AThingsInWoodsCharacter* PlayerPawn = Cast<AThingsInWoodsCharacter>(this->GetOwner());
	if (PlayerPawn != nullptr)
	{
		// drop
		PlayerPawn->PlantObject();
	}
}

void AMonitorCamera::execSecondary()
{
	AThingsInWoodsCharacter* PlayerPawn = Cast<AThingsInWoodsCharacter>(this->GetOwner());
	if (PlayerPawn != nullptr)
	{

	}
}

void AMonitorCamera::ActivateObject()
{
	//Register on the computer objects

}

void AMonitorCamera::DisableObject()
{
	//Reset
	this->ActiveCameraIndex = -1;
	this->SwitchCamera();
}

void AMonitorCamera::BeginPlay()
{
	Super::BeginPlay();
	this->DisableObject();
}

void AMonitorCamera::RegisterCamera(ABaseItem* CameraActor)
{
	for (int i = 0; i < this->ActiveCameras.Num(); i++)
	{
		if (this->ActiveCameras[i] == CameraActor)
		{
			//Already registered, cancel
			return;
		}
	}

	this->ActiveCameras.Add(CameraActor);
	this->ActivateCamera(this->ActiveCameras.Num() - 1);
}

void AMonitorCamera::UnRegisterCamera(ABaseItem* CameraActor)
{
	for (int i = 0; i < this->ActiveCameras.Num(); i++)
	{
		if (this->ActiveCameras[i] == CameraActor)
		{
			this->ActiveCameras.RemoveAt(i);
			this->SwitchCamera();
			return;
		}
	}
}

void AMonitorCamera::ActivateCamera(int index)
{
	if (index < this->ActiveCameras.Num())
	{
		if (this->ActiveCameras[index]->IsValidLowLevel())
		{
			ACamera* ActiveCamera = Cast<ACamera>(this->ActiveCameras[index]);
			if (ActiveCamera != nullptr)
			{
				this->GetMesh()->SetMaterial(0, ActiveCamera->CameraMaterialInstance);
			}
			else
			{
				return;
			}
		}
	}
}

void AMonitorCamera::SwitchCamera()
{
	if (this->ActiveCameras.Num() == 0)
	{
		//No Cameras, set offline Material
		this->GetMesh()->SetMaterial(0, this->CameraOfflineMaterial);
	}
	else
	{
		this->ActiveCameraIndex += 1;
		if (this->ActiveCameraIndex >= this->ActiveCameras.Num())
		{
			this->ActiveCameraIndex = 0;
		}

		this->ActivateCamera(this->ActiveCameraIndex);
	}
}

void AMonitorCamera::UseObject()
{
	this->SwitchCamera();
}