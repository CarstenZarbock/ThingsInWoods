// Fill out your copyright notice in the Description page of Project Settings.

#include "ThingsInWoods.h"
#include "ThingsInWoodsSpectator.h"
#include "ThingsInWoodsCharacter.h"
#include "Engine.h"

AThingsInWoodsSpectator::AThingsInWoodsSpectator()
{
	SpringarmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("Springarm"));
	SpringarmComponent->AttachTo(RootComponent);
	SpringarmComponent->bUsePawnControlRotation = true;
	
	FRotator Rot;
	Rot.Roll = 0;
	Rot.Pitch = -40;
	Rot.Yaw = 0;

	SpringarmComponent->TargetArmLength = 300;
	SpringarmComponent->bDoCollisionTest = true;
	SpringarmComponent->ProbeChannel = ECollisionChannel::ECC_Camera;
	SpringarmComponent->bAutoActivate = true;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->AttachTo(this->SpringarmComponent);
	FirstPersonCameraComponent->RelativeLocation = FVector(0, 0, 0); // Position the camera
	//FirstPersonCameraComponent->bUsePawnControlRotation = true;
	FirstPersonCameraComponent->bAutoActivate = true;

	SpringarmComponent->SetRelativeRotation(Rot.Quaternion());
}

void AThingsInWoodsSpectator::BeginPlay()
{
	Super::BeginPlay();
}

void AThingsInWoodsSpectator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AThingsInWoodsSpectator::AttachToPlayerPawn(AThingsInWoodsCharacter* Pawn)
{
	if (Pawn != nullptr && Pawn->IsValidLowLevel())
	{
		this->AttachToActor(Pawn, FAttachmentTransformRules::KeepRelativeTransform);
		this->AttachedPawn = Pawn;
	}
}

AThingsInWoodsCharacter* AThingsInWoodsSpectator::GetNextPlayer()
{
	//Get all player characters
	for (TActorIterator<AThingsInWoodsCharacter> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		AThingsInWoodsCharacter* OtherCharacter = *ActorItr;
		if (OtherCharacter->IsValidLowLevel() && OtherCharacter->IsAlive() && OtherCharacter != this->AttachedPawn)
		{
			return OtherCharacter;
		}
	}

	return nullptr;
}