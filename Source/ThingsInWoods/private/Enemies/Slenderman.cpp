// Fill out your copyright notice in the Description page of Project Settings.

#include "ThingsInWoods.h"
#include "Slenderman.h"

// Sets default values
ASlenderman::ASlenderman()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ASlenderman::BeginPlay()
{
	Super::BeginPlay();
	this->GetAllActivePlayers();
	
}

// Called every frame
void ASlenderman::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
	if (this->isActive)
	{
		this->CheckForPlayerAngles(DeltaTime);
		this->HandleTeleport();
	}

}

// Called to bind functionality to input
void ASlenderman::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ASlenderman::GetAllActivePlayers()
{
	ActivePlayers.Empty();

	for (TActorIterator<AThingsInWoodsCharacter> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		AThingsInWoodsCharacter* PlayerCharacter = *ActorItr;
		if (PlayerCharacter != nullptr && PlayerCharacter->IsAlive())
		{
			ActivePlayers.Add(PlayerCharacter);
		}
	}
}

void ASlenderman::TeleportToPlayer()
{
	this->GetAllActivePlayers();

	int PlayerNum = this->ActivePlayers.Num();

	int PlayerIndex = FMath::RandRange(0, PlayerNum - 1);
	if (PlayerIndex < PlayerNum)
	{
		AThingsInWoodsCharacter* PlayerTarget = this->ActivePlayers[PlayerIndex];

		if (PlayerTarget->IsValidLowLevel())
		{
			//Get Player Backwards Vector * Distance -> Random Point
			float fRandom = FMath::FRandRange(-65, 65);
			FVector ForwardVec = (PlayerTarget->GetActorForwardVector() * -1).RotateAngleAxis(fRandom, FVector(0, 0, 1));
			FVector VecA = PlayerTarget->GetActorLocation();
			FVector VecB = ForwardVec * this->SpawnDistance;
			FVector VecC = VecA + VecB;

			UNavigationSystem* const NavSys = GetWorld()->GetNavigationSystem();
			FNavLocation Location;

			if (NavSys != nullptr)
			{
				if (NavSys->GetRandomReachablePointInRadius(VecC, this->SpawnDistanceRange, Location))
				{
					this->SetActorLocation(Location.Location);
				}
			}

		}
	}
}

void ASlenderman::HandleTeleport()
{
	if (!this->isScaring)
	{
		if (GetGameTimeSinceCreation() > this->NextTeleportTimer)
		{
			this->TeleportToPlayer();
			this->NextTeleportTimer = GetGameTimeSinceCreation() + this->SpawnRate;
		}
	}
}

void ASlenderman::CheckForPlayerAngles(float DeltaTime)
{
	int PlayerNum = this->ActivePlayers.Num();
	bool tempScare = false;

	for (int i = 0; i < PlayerNum; i++)
	{
		if (ActivePlayers[i]->IsValidLowLevel())
		{
			if (ActivePlayers[i]->IsAlive())
			{
				FVector VecA, VecB;
				VecA = this->GetActorLocation() - ActivePlayers[i]->GetActorLocation();
				VecB = ActivePlayers[i]->GetFirstPersonCameraComponent()->GetForwardVector();
				
				float Angle = FMath::RadiansToDegrees(FMath::Acos((FVector::DotProduct(VecA, VecB) / (VecA.Size() * VecB.Size()))));

				if (Angle < this->KillAngle)
				{
					GEngine->AddOnScreenDebugMessage(0, 1.0f, FColor::Blue, FString::FromInt(Angle));
					//Scare Player
					//Calculate Scare Amount
					//(Distance / 1000) * (KillAngle / Angle)
					//Get Distance

					float distance = FVector::Dist(this->GetActorLocation(), ActivePlayers[i]->GetActorLocation());
					if (distance < this->ScareRange)
					{
						if (Angle <= 0.0f)
						{
							Angle = 0.1f;
						}

						if (distance <= 0.0f)
						{
							distance = 0.1f;
						}
						//Distance 5000
						//-5000
						//+10000
						//5000
						//5 *
						float Base_Distance_Value = (distance * -1) + this->ScareRange;
						float Base_Angle_Value = (KillAngle / Angle) / 100;
						float amount = (((Base_Distance_Value / 1000) + Base_Angle_Value) * 1.0f);
						//amount *= DeltaTime;
						ActivePlayers[i]->ReceiveSlenderScare(amount);
						tempScare = true;
					}
				}
			}
		}
	}

	this->isScaring = tempScare;
}

void ASlenderman::SetStats(bool Active, float AFactor)
{
	this->isActive = Active;
	this->AggressiveFactor = AFactor;

	if (this->AggressiveFactor > 0.0f)
	{
		this->ScareAggressionFactor = 1.0f + ((this->AggressiveFactor / 10.0f) * 2.0f);
	}

	this->SpawnDistance = this->ScareRange - (this->AggressiveFactor * 300.0f);
	this->SpawnDistanceRange = this->AggressiveFactor * 10.0f;
	this->SpawnRate = 30.0f - (this->AggressiveFactor * 2.0f);
}