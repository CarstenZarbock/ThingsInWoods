// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "ThingsInWoods.h"
#include "BaseItem.h"
#include "ThingsInWoodsCharacter.h"
#include "UnrealNetwork.h"

// Sets default values
ABaseItem::ABaseItem()
{
	bReplicates = true;
	PrimaryActorTick.bCanEverTick = true;
	//RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;
	//Mesh->AttachTo(RootComponent);

	AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("Audio"));
	AudioComponent->SetupAttachment(this->GetMesh());
}

// Called when the game starts or when spawned
void ABaseItem::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABaseItem::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
	this->HandleUsage();
	this->CustomTick();
	this->HandlePhysicPositionCheck();
}

void ABaseItem::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABaseItem, LastKnownTransform);
}

void ABaseItem::OnRep_Flag()
{
	if (this->GetObjectState() == EObjectState::OS_WORLD)
	{
		if (this->GetMesh()->IsSimulatingPhysics())
		{
			this->GetMesh()->SetWorldLocation(this->LastKnownTransform.GetLocation(), false, (FHitResult*)nullptr, ETeleportType::TeleportPhysics);
			this->GetMesh()->SetWorldRotation(this->LastKnownTransform.GetRotation(), false, (FHitResult*)nullptr, ETeleportType::TeleportPhysics);
		}
	}
}

void ABaseItem::HandlePhysicPositionCheck()
{
	if (HasAuthority())
	{
		if (this->GetObjectState() == EObjectState::OS_WORLD)
		{
			if (this->GetMesh()->IsSimulatingPhysics())
			{
				//Check Position
				FVector CompLocation = this->GetMesh()->GetComponentLocation();
				FRotator CompRotation = this->GetMesh()->GetComponentRotation();
				float fDistance = FVector::Dist(CompLocation, this->LastKnownTransform.GetLocation());

				if (fDistance > 100.0f)
				{
					//Set LastKnownPosition (Replicated)
					this->LastKnownTransform.SetLocation(CompLocation);
					this->LastKnownTransform.SetRotation(CompRotation.Quaternion());
				}
				
			}
		}
	}
}

//------------------------------------------------------------------------
void ABaseItem::execSetState(EObjectState state, FVector Location)
{
	switch (state)
	{
	case EObjectState::OS_INVENTORY:
		this->isOwned = true;
		Mesh->SetSimulatePhysics(false);
		Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Mesh->SetRelativeTransform(MeshRelativeTransform);
		this->DisableObject();
		this->SetActorHiddenInGame(false);
		this->GetMesh()->SetOwnerNoSee(false);
		this->GetMesh()->SetOnlyOwnerSee(true);
		this->ObjectState = state;
		break;
	
	case EObjectState::OS_STACKED:
		this->isOwned = true;
		Mesh->SetSimulatePhysics(false);
		Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Mesh->SetRelativeTransform(MeshRelativeTransform);
		this->DisableObject();
		this->SetActorHiddenInGame(true);
		this->ObjectState = state;
		break;
	
	case EObjectState::OS_WORLD:
		this->isOwned = false;
		this->SetActorLocation(Location, false);
		this->LastKnownTransform.SetLocation(Location);
		Mesh->SetSimulatePhysics(true); //!!Fix Physics replication first before changing!!
		Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		Mesh->SetOnlyOwnerSee(false);
		Mesh->SetOwnerNoSee(false);
		this->SetActorHiddenInGame(false);
		this->ObjectState = state;
		break;

	case EObjectState::OS_PLACED:
		this->isOwned = false;
		this->SetActorLocation(Location, false);
		this->LastKnownTransform.SetLocation(Location);
		Mesh->SetSimulatePhysics(true); //!!Fix Physics replication first before changing!!
		Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		Mesh->SetOnlyOwnerSee(false);
		Mesh->SetOwnerNoSee(false);
		this->SetActorHiddenInGame(false);
		this->ObjectState = state;
		
		break;
	
	default:
		break;
	}
}

bool ABaseItem::SetState_Validate(EObjectState state, FVector Location)
{
	return true;
}

void ABaseItem::SetState_Implementation(EObjectState state, FVector Location)
{
	this->execSetState(state, Location);

	if (Role < ROLE_Authority)
	{
		this->Server_SetState(state, Location);
	}
}

bool ABaseItem::Server_SetState_Validate(EObjectState state, FVector Location)
{
	return true;
}

void ABaseItem::Server_SetState_Implementation(EObjectState state, FVector Location)
{
	this->execSetState(state, Location);
}
//------------------------------------------------------------------------
void ABaseItem::execPrimary()
{

}

void ABaseItem::execSecondary()
{

}

void ABaseItem::Primary()
{
	if (this->GetPrimarySkill()->GetIsOverTime())
	{
		this->isCurrentlyUsed = true;
		this->isPrimaryUsed = true;
		this->NextUsageHitTime = GetGameTimeSinceCreation() + 0.1f;
		this->currentUsageProgression = 0.0f;
	}
	else
	{
		this->execPrimary();
	}
}

void ABaseItem::StopPrimary()
{
	this->isCurrentlyUsed = false;
}

void ABaseItem::Secondary()
{
	if (this->GetSecondarySkill()->GetIsOverTime())
	{
		this->isCurrentlyUsed = true;
		this->isPrimaryUsed = false;
		this->NextUsageHitTime = GetGameTimeSinceCreation() + 0.1f;
		this->currentUsageProgression = 0.0f;
	}
	else
	{
		this->execSecondary();
	}
}

void ABaseItem::StopSecondary()
{
	this->isCurrentlyUsed = false;
}

void ABaseItem::HandleUsage()
{
	if (this->isCurrentlyUsed)
	{
		if (GetGameTimeSinceCreation() > this->NextUsageHitTime)
		{
			if (this->isPrimaryUsed)
			{
				this->currentUsageProgression += this->GetPrimarySkill()->GetTimeFactor();
			}else
			{
				this->currentUsageProgression += this->GetSecondarySkill()->GetTimeFactor();
			}

			this->NextUsageHitTime = GetGameTimeSinceCreation() + 0.1f;
		}
	}
}

void ABaseItem::CustomTick()
{

}

void ABaseItem::ActivateObject()
{

}

void ABaseItem::DisableObject()
{

}

void ABaseItem::UseObject()
{

}

//------------------------------------------------------------------------
void ABaseItem::execPlaySound(USoundBase* Sound, UAudioComponent* Audio_Component, bool ifAvailable)
{
	if (ifAvailable)
	{
		if (Audio_Component->IsPlaying())
		{
			//We're already playing, cancel new play
			return;
		}
	}

	if (Audio_Component->IsValidLowLevel() && Sound->IsValidLowLevel())
	{
		Audio_Component->SetSound(Sound);
		Audio_Component->Play();
	}
}

bool ABaseItem::PlaySound_Validate(USoundBase* Sound, UAudioComponent* Audio_Component, bool ifAvailable)
{
	return true;
}

void ABaseItem::PlaySound_Implementation(USoundBase* Sound, UAudioComponent* Audio_Component, bool ifAvailable)
{
	if (!HasAuthority())
	{
		this->Server_PlaySound(Sound, Audio_Component, ifAvailable);
	}
	else
	{
		this->Server_PlaySound(Sound, Audio_Component, ifAvailable);
	}
}

bool ABaseItem::Server_PlaySound_Validate(USoundBase* Sound, UAudioComponent* Audio_Component, bool ifAvailable)
{
	return true;
}

void ABaseItem::Server_PlaySound_Implementation(USoundBase* Sound, UAudioComponent* Audio_Component, bool ifAvailable)
{
	this->Multi_PlaySound(Sound, Audio_Component, ifAvailable);
}

bool ABaseItem::Multi_PlaySound_Validate(USoundBase* Sound, UAudioComponent* Audio_Component, bool ifAvailable)
{
	return true;
}

void ABaseItem::Multi_PlaySound_Implementation(USoundBase* Sound, UAudioComponent* Audio_Component, bool ifAvailable)
{
	this->execPlaySound(Sound, Audio_Component, ifAvailable);
}