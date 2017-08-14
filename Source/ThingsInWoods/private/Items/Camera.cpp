// Fill out your copyright notice in the Description page of Project Settings.

#include "ThingsInWoods.h"
#include "Camera.h"
#include "MonitorCamera.h"
#include "ThingsInWoodsCharacter.h"
#include "Engine.h"


ACamera::ACamera()
{
	SceneCapture = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("SceneCapture"));
	SceneCapture->SetupAttachment(RootComponent);
}

void ACamera::execPrimary()
{
	AThingsInWoodsCharacter* PlayerPawn = Cast<AThingsInWoodsCharacter>(this->GetOwner());
	if (PlayerPawn != nullptr)
	{
		// drop
		PlayerPawn->PlantObject();
	}
}

void ACamera::execSecondary()
{
	AThingsInWoodsCharacter* PlayerPawn = Cast<AThingsInWoodsCharacter>(this->GetOwner());
	if (PlayerPawn != nullptr)
	{

	}
}

void ACamera::ActivateObject()
{
	//Register on the computer objects
	this->Register();
}

void ACamera::DisableObject()
{
	//Unregister on the computer objects
	this->UnRegister();
}

void ACamera::BeginPlay()
{
	Super::BeginPlay();

	this->CameraMaterialInstance = UMaterialInstanceDynamic::Create(this->CameraMaterial, this);

	this->RenderTarget = NewObject<UTextureRenderTarget2D>();

	this->RenderTarget->bHDR = false;
	this->RenderTarget->ClearColor = FLinearColor::White;
	this->RenderTarget->TargetGamma = 1.0f;
	this->RenderTarget->InitAutoFormat(320, 240); //auto init from value bHDR

	this->CameraMaterialInstance->SetTextureParameterValue("TextureParameter", this->RenderTarget);
	this->SceneCapture->TextureTarget = this->RenderTarget;

	this->DisableObject();
}

void ACamera::Register()
{
	//Get all characters
	for (TActorIterator<AMonitorCamera> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		AMonitorCamera* MonitorActor = *ActorItr;
		if (MonitorActor != nullptr)
		{
			MonitorActor->RegisterCamera(this);
		}
	}
}

void ACamera::UnRegister()
{
	for (TActorIterator<AMonitorCamera> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		AMonitorCamera* MonitorActor = *ActorItr;
		if (MonitorActor != nullptr)
		{
			MonitorActor->UnRegisterCamera(this);
		}
	}
}