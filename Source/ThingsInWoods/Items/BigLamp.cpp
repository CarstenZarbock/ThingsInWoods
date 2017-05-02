// Fill out your copyright notice in the Description page of Project Settings.

#include "ThingsInWoods.h"
#include "BigLamp.h"
#include "ThingsInWoodsCharacter.h"


ABigLamp::ABigLamp()
{
	Spotlight = CreateDefaultSubobject<USpotLightComponent>(TEXT("Spotlight"));
	Spotlight->SetupAttachment(RootComponent);
}

void ABigLamp::execPrimary()
{
	AThingsInWoodsCharacter* PlayerPawn = Cast<AThingsInWoodsCharacter>(this->GetOwner());
	if (PlayerPawn != nullptr)
	{
		// drop
		PlayerPawn->PlantObject();
	}
}

void ABigLamp::execSecondary()
{
	AThingsInWoodsCharacter* PlayerPawn = Cast<AThingsInWoodsCharacter>(this->GetOwner());
	if (PlayerPawn != nullptr)
	{
		
	}
}

void ABigLamp::ActivateObject()
{
	Spotlight->SetIntensity(this->Intensity);
}

void ABigLamp::DisableObject()
{
	Spotlight->SetIntensity(0);
}

void ABigLamp::BeginPlay()
{
	Super::BeginPlay();
	this->DisableObject();
}