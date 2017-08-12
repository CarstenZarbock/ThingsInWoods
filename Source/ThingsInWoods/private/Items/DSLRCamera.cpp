// Fill out your copyright notice in the Description page of Project Settings.

#include "ThingsInWoods.h"
#include "DSLRCamera.h"
#include "ThingsInWoodsCharacter.h"
#include "Slenderman/Slender_Target_Spawn.h"

ADSLRCamera::ADSLRCamera()
{
	SceneCapture = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("SceneCapture"));
	SceneCapture->SetupAttachment(this->GetMesh());

	FlashLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("Flash"));
	FlashLight->SetupAttachment(this->GetMesh());
}

void ADSLRCamera::CustomTick()
{
	//Tick
	if (this->hasFlashed)
	{
		if (GetGameTimeSinceCreation() > this->FlashTimer)
		{
			this->FlashLight->SetIntensity(0.0f);
			this->FlashTimer = GetGameTimeSinceCreation() + 99999.9f; //maybe fix
		}

		if (GetGameTimeSinceCreation() > this->ReloadTimer)
		{
			this->hasFlashed = false;
		}
	}
}

void ADSLRCamera::HandleTargets()
{
	AThingsInWoodsCharacter* Player;
	Player = Cast<AThingsInWoodsCharacter>(this->GetOwner());
	if (Player != nullptr)
	{
		for (TActorIterator<ASlender_Target_Spawn> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			ASlender_Target_Spawn* CurrentTarget = *ActorItr;
			if (CurrentTarget != nullptr && CurrentTarget->isSpawned && !CurrentTarget->isCompleted)
			{
				float distance = FVector::Dist(Player->GetActorLocation(), CurrentTarget->GetActorLocation());
				if (distance < 1000) //Snap Distance
				{
					//Get Angle
					FVector VecA, VecB;
					VecA = CurrentTarget->GetActorLocation() - Player->GetActorLocation();
					VecB = Player->GetFirstPersonCameraComponent()->GetForwardVector();

					float Angle = FMath::RadiansToDegrees(FMath::Acos((FVector::DotProduct(VecA, VecB) / (VecA.Size() * VecB.Size()))));
					if (Angle < 40) //Snap Angle
					{
						CurrentTarget->SetCompleted();
					}
				}
			}
		}
	}
}

void ADSLRCamera::execPrimary()
{
	//Flash
	if (!this->hasFlashed)
	{
		//Flash Camera
		this->FlashLight->SetIntensity(this->FlashIntensity);
		this->FlashTimer = GetGameTimeSinceCreation() + this->FlashTime;
		this->ReloadTimer = GetGameTimeSinceCreation() + this->ReloadTime;
		this->hasFlashed = true;

		//Handle Targets
		this->HandleTargets();

		this->PlaySound(this->Flash_Loading, this->AudioComponent, false);
	}

	return;
}

void ADSLRCamera::execSecondary()
{

}

void ADSLRCamera::ActivateObject()
{
	//this->SceneCapture->CaptureScene.Activate;
	this->GetMesh()->SetMaterial(DisplayMaterialElementIndex, this->CameraMaterialInstance);

	this->PlaySound(this->Toggle_On, this->AudioComponent, false);
}

void ADSLRCamera::DisableObject()
{
	//this->SceneCapture->CaptureScene;
	this->GetMesh()->SetMaterial(DisplayMaterialElementIndex, this->CameraMaterialInstance);
	//Add Offline Material

	this->PlaySound(this->Toggle_Off, this->AudioComponent, false);
}

void ADSLRCamera::BeginPlay()
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