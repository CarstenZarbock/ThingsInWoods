// Fill out your copyright notice in the Description page of Project Settings.

#include "ThingsInWoods.h"
#include "MotionDetectorLaser.h"
#include "MonitorLaser.h"
#include "ThingsInWoodsCharacter.h"


AMotionDetectorLaser::AMotionDetectorLaser()
{

}

void AMotionDetectorLaser::CustomTick()
{
	//Tick
	if (this->isScanning)
	{
		//Scan
		this->Scan();
	}
}

void AMotionDetectorLaser::execPrimary()
{
	AThingsInWoodsCharacter* PlayerPawn = Cast<AThingsInWoodsCharacter>(this->GetOwner());
	if (PlayerPawn != nullptr)
	{
		// drop
		PlayerPawn->PlantObject();
	}
}

void AMotionDetectorLaser::execSecondary()
{
	AThingsInWoodsCharacter* PlayerPawn = Cast<AThingsInWoodsCharacter>(this->GetOwner());
	if (PlayerPawn != nullptr)
	{

	}
}

void AMotionDetectorLaser::ActivateObject()
{
	//Register on the computer objects
	this->Register();
	this->GetAllLaserNear();
	this->isScanning = true;
}

void AMotionDetectorLaser::GetAllLaserNear()
{
	//Get all characters
	for (TActorIterator<AMotionDetectorLaser> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		AMotionDetectorLaser* MonitorActor = *ActorItr;
		if (MonitorActor != nullptr && MonitorActor != this && MonitorActor->isActive())
		{
			//Check Distance
			if (MonitorActor->GetDistanceTo(this) <= DetectionDistance)
			{
				//Cast Raycast
				FHitResult RV_Hit(ForceInit);
				TArray<AActor*> ActorsToIgnore;
				ActorsToIgnore.Add(this);
				ActorsToIgnore.Add(this->GetOwner());

				RV_Hit = TraceLine(this->GetActorLocation(), MonitorActor->GetMesh()->GetComponentLocation(), true, ActorsToIgnore);
				if (RV_Hit.GetActor() == MonitorActor)
				{
					//Register
					FLaserItem NewLaser;
					NewLaser.LaserActor = MonitorActor;
					//----------------
					UParticleSystemComponent* ParticleTemp;
					ParticleTemp = UGameplayStatics::SpawnEmitterAttached
					(this->LaserParticle,
						this->GetMesh(),
						NAME_None,
						this->GetMesh()->GetComponentLocation(),
						this->GetMesh()->GetComponentRotation(),
						EAttachLocation::KeepRelativeOffset,
						false);
					
					ParticleTemp->SetActorParameter("BeamSource", this);
					ParticleTemp->SetActorParameter("BeamTarget", MonitorActor);

					//---------------
					NewLaser.ActiveParticle = ParticleTemp;
					
					this->RegisteredLaser.Add(NewLaser);
					MonitorActor->PassiveLaser.Add(this);

					FString Message;
					Message.Append("Actor Found: ");
					Message.Append(MonitorActor->GetName());
					GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, Message);
				}
				else
				{
					GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, "else");
				}

			}
			else
			{
				GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, "Distance Wrong");
			}
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, "!= null != this");
		}
	}
}

void AMotionDetectorLaser::RemoveFromRegisteredLaser(AActor* LaserToRemove)
{
	for (int i = 0; i < this->RegisteredLaser.Num(); i++)
	{
		if (this->RegisteredLaser[i].LaserActor == LaserToRemove)
		{
			this->RegisteredLaser[i].ActiveParticle->DestroyComponent();
			this->RegisteredLaser.RemoveAt(i);
			return;
		}
	}
}

void AMotionDetectorLaser::ClearRegisteredLaser()
{
	for (int i = 0; i < this->RegisteredLaser.Num(); i++)
	{
			this->RegisteredLaser[i].ActiveParticle->DestroyComponent();
	}
}

void AMotionDetectorLaser::UnRegisterOnPassiveLaser()
{
	for (int i = 0; i < this->PassiveLaser.Num(); i++)
	{
		if (this->PassiveLaser[i]->IsValidLowLevel())
		{
			this->PassiveLaser[i]->RemoveFromRegisteredLaser(this);
		}
	}
}

void AMotionDetectorLaser::DisableObject()
{
	//Unregister on the computer objects
	this->UnRegister();
	this->UnRegisterOnPassiveLaser();
	this->ClearRegisteredLaser();

	//Delete all registered Laser
	this->RegisteredLaser.Empty();
	this->PassiveLaser.Empty();

	//Disable Scanning
	this->isScanning = false;
}

void AMotionDetectorLaser::ExecuteScans()
{
	for (int i = 0; i < this->RegisteredLaser.Num(); i++)
	{
		if (this->RegisteredLaser[i].LaserActor->IsValidLowLevel())
		{
			//------- Actuall Scan
			//Cast Raycast
			FHitResult RV_Hit(ForceInit);
			TArray<AActor*> ActorsToIgnore;
			ActorsToIgnore.Add(this);

			RV_Hit = TraceLine(this->GetActorLocation(), this->RegisteredLaser[i].LaserActor->GetActorLocation(), true, ActorsToIgnore);
			if (RV_Hit.GetActor() != this->RegisteredLaser[i].LaserActor && RV_Hit.GetActor() != nullptr)
			{
				//Register
					FString Message;
				Message.Append("Scan Found: ");
				Message.Append(RV_Hit.GetActor()->GetName());
				GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, Message);
			}
			//---------------------------------------------------
		}
	}
}

void AMotionDetectorLaser::Scan()
{
	if (GetGameTimeSinceCreation() > this->NextScanTime)
	{
		this->ExecuteScans();
		this->NextScanTime = GetGameTimeSinceCreation() + this->ScanFactorTime;
	}
}

void AMotionDetectorLaser::BeginPlay()
{
	Super::BeginPlay();
	this->DisableObject();
}

void AMotionDetectorLaser::Register()
{
	//Get all characters
	for (TActorIterator<AMonitorLaser> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		AMonitorLaser* MonitorActor = *ActorItr;
		if (MonitorActor != nullptr)
		{
			MonitorActor->RegisterScanner(this);
			this->RegisteredMonitor.Add(MonitorActor);
			GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, "Registered");
		}
	}
}

void AMotionDetectorLaser::UnRegister()
{
	for (int i = 0; i < this->RegisteredMonitor.Num(); i++)
	{
		if (this->RegisteredMonitor[i]->IsValidLowLevel())
		{
			AMonitorLaser* MonitorActor = Cast<AMonitorLaser>(this->RegisteredMonitor[i]);

			if (MonitorActor != nullptr)
			{
				MonitorActor->UnRegisterScanner(this);
			}
		}
	}

	this->RegisteredMonitor.Empty();
}

//////////////////////////////////////////////////////////////////////////
// Temp / Other
FHitResult AMotionDetectorLaser::TraceLine(FVector Start, FVector End, bool Debug, TArray<AActor*> ActorsToIgnore)
{
	FCollisionQueryParams RV_TraceParams = FCollisionQueryParams(FName(TEXT("RV_Trace")), true, this);
	RV_TraceParams.bTraceComplex = false;
	RV_TraceParams.bTraceAsyncScene = true;
	RV_TraceParams.bReturnPhysicalMaterial = false;
	RV_TraceParams.AddIgnoredActors(ActorsToIgnore);

	if (Debug)
	{
		const FName TraceTag("TraceTag");
		GetWorld()->DebugDrawTraceTag = TraceTag;
		RV_TraceParams.TraceTag = TraceTag;
	}

	//Re-initialize hit info
	FHitResult RV_Hit(ForceInit);

	GetWorld()->LineTraceSingleByChannel(
		RV_Hit,		       //result
		Start,			  //start
		End,			 //end
		ECC_Visibility, //collision channel
		RV_TraceParams
	);

	if (Debug)
	{
		DrawDebugLine(
			GetWorld(),
			Start,
			RV_Hit.ImpactPoint,
			FColor(255, 0, 0),
			false, -1, 0,
			12.333
		);
	}

	return RV_Hit;
}