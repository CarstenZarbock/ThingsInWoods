// Fill out your copyright notice in the Description page of Project Settings.

#include "ThingsInWoods.h"
#include "Flashlight.h"
#include "ThingsInWoodsCharacter.h"


AFlashlight::AFlashlight()
{
	FlashLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("Flash"));
	FlashLight->SetupAttachment(this->GetMesh());
	VolumetricLight = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VolumetricLightShaft"));
	VolumetricLight->SetupAttachment(this->GetMesh());
}

void AFlashlight::CustomTick()
{
	//Tick
}

void AFlashlight::execPrimary()
{
	//Flash
	if (!this->isOn)
	{
		this->ActivateObject();
	}
	else
	{
		this->DisableObject();
	}

	return;
}

void AFlashlight::execSecondary()
{

}

void AFlashlight::ActivateObject()
{
	if (!this->isOn)
	{
		this->FlashLight->SetIntensity(this->LightIntensity);
		this->VolumetricLight->SetVisibility(true);
		this->isOn = true;
		this->FlashlightMaterialInstance->SetScalarParameterValue("Emissive", 10.0f);
		this->PlaySound(this->Toggle, this->AudioComponent, false);
	}
}

void AFlashlight::DisableObject()
{
	if (this->isOn)
	{
		this->FlashLight->SetIntensity(0.0f);
		this->VolumetricLight->SetVisibility(false);
		this->isOn = false;
		this->FlashlightMaterialInstance->SetScalarParameterValue("Emissive", 0.0f);
		this->PlaySound(this->Toggle, this->AudioComponent, false);
	}
}

void AFlashlight::BeginPlay()
{
	Super::BeginPlay();
	this->FlashlightMaterialInstance = UMaterialInstanceDynamic::Create(this->GetMesh()->GetMaterial(0), this);
	this->GetMesh()->SetMaterial(0, this->FlashlightMaterialInstance);
	this->isOn = true; //Workaround
	this->DisableObject();
}