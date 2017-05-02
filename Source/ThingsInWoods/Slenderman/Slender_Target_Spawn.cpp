// Fill out your copyright notice in the Description page of Project Settings.

#include "ThingsInWoods.h"
#include "Slender_Target_Spawn.h"


// Sets default values
ASlender_Target_Spawn::ASlender_Target_Spawn()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
	RootComponent = RootScene;

	this->isSpawned = false;
	this->SetActorHiddenInGame(true);
	this->SetActorEnableCollision(false);
	this->PendingComplete = false;
	this->isCompleted = false;
}

// Called when the game starts or when spawned
void ASlender_Target_Spawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASlender_Target_Spawn::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void ASlender_Target_Spawn::SpawnTarget()
{
	this->Spawn_Activate();
}

void ASlender_Target_Spawn::Spawn_Activate()
{
	this->isSpawned = true;
	this->isCompleted = false;
	RootComponent->SetVisibility(true, true);
	this->SetActorHiddenInGame(false);
	this->SetActorEnableCollision(true);
}

void ASlender_Target_Spawn::SetCompleted()
{
	if (!this->isCompleted && this->isSpawned)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, "TARGET FOUND");
		this->isCompleted = true;
		this->PendingComplete = true;
	}
}
