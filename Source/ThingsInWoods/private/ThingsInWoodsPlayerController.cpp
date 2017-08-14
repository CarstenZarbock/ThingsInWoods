// Fill out your copyright notice in the Description page of Project Settings.

#include "ThingsInWoods.h"
#include "ThingsInWoodsPlayerController.h"
#include "UnrealNetwork.h"
#include "ThingsInWoodsCharacter.h"
#include "ThingsInWoodsSpectator.h"
#include "ThingsInWoodsPlayerState.h"
#include "ThingsInWoodsHUD.h"

AThingsInWoodsPlayerController::AThingsInWoodsPlayerController()
{

}

void AThingsInWoodsPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;
}

void AThingsInWoodsPlayerController::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AThingsInWoodsPlayerController, PossessedPlayerPawn);
	DOREPLIFETIME(AThingsInWoodsPlayerController, PossessedSpectatorPawn);
}

void AThingsInWoodsPlayerController::SetPawn(APawn* InPawn)
{
	Super::SetPawn(InPawn);
	
	if (Cast<AThingsInWoodsCharacter>(InPawn) != nullptr)
	{
		this->PossessedPlayerPawn = Cast<AThingsInWoodsCharacter>(InPawn);
		this->bPawnPossessed = true;

		this->bSpectatorPossessed = false;
		this->SetSpectatorPawn(nullptr);
		return;
	}

	if (Cast<AThingsInWoodsSpectator>(InPawn) != nullptr)
	{
		this->SetSpectatorPawn(Cast<AThingsInWoodsSpectator>(InPawn));
		this->bPawnPossessed = false;

		this->bSpectatorPossessed = true;
		this->PossessedPlayerPawn = nullptr;
		return;
	}
}

void AThingsInWoodsPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAction("Jump", IE_Pressed, this, &AThingsInWoodsPlayerController::Jump);
	InputComponent->BindAction("Jump", IE_Released, this, &AThingsInWoodsPlayerController::StopJump);
	InputComponent->BindAction("Sprint", IE_Pressed, this, &AThingsInWoodsPlayerController::Sprint);
	InputComponent->BindAction("Sprint", IE_Released, this, &AThingsInWoodsPlayerController::StopSprint);

	InputComponent->BindAction("Primary", IE_Pressed, this, &AThingsInWoodsPlayerController::Primary);
	InputComponent->BindAction("Primary", IE_Released, this, &AThingsInWoodsPlayerController::StopPrimary);
	InputComponent->BindAction("Secondary", IE_Pressed, this, &AThingsInWoodsPlayerController::Secondary);
	InputComponent->BindAction("Secondary", IE_Released, this, &AThingsInWoodsPlayerController::StopSecondary);

	InputComponent->BindAction("Use", IE_Pressed, this, &AThingsInWoodsPlayerController::Use);
	InputComponent->BindAction("Use", IE_Released, this, &AThingsInWoodsPlayerController::StopUse);

	InputComponent->BindAction("Nightvision", IE_Pressed, this, &AThingsInWoodsPlayerController::ToggleNightVision);

	InputComponent->BindAxis("MoveForward", this, &AThingsInWoodsPlayerController::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &AThingsInWoodsPlayerController::MoveRight);

	InputComponent->BindAxis("Turn", this, &AThingsInWoodsPlayerController::TurnAtRate);
	InputComponent->BindAxis("TurnRate", this, &AThingsInWoodsPlayerController::TurnAtRate);
	InputComponent->BindAxis("LookUp", this, &AThingsInWoodsPlayerController::LookUpAtRate);
	InputComponent->BindAxis("LookUpRate", this, &AThingsInWoodsPlayerController::LookUpAtRate);

	InputComponent->BindAction("Inventory_Key_1", IE_Pressed, this, &AThingsInWoodsPlayerController::Inventory_Select<0>);
	InputComponent->BindAction("Inventory_Key_2", IE_Pressed, this, &AThingsInWoodsPlayerController::Inventory_Select<1>);
	InputComponent->BindAction("Inventory_Key_3", IE_Pressed, this, &AThingsInWoodsPlayerController::Inventory_Select<2>);
	InputComponent->BindAction("Inventory_Key_4", IE_Pressed, this, &AThingsInWoodsPlayerController::Inventory_Select<3>);
	InputComponent->BindAction("Inventory_Key_5", IE_Pressed, this, &AThingsInWoodsPlayerController::Inventory_Select<4>);

	InputComponent->BindAction("Inventory_Drop", IE_Pressed, this, &AThingsInWoodsPlayerController::Inventory_Drop);
}

void AThingsInWoodsPlayerController::Inventory_Drop()
{
	if (bPawnPossessed)
	{
		this->PossessedPlayerPawn->InventoryDrop();
	}
}

void AThingsInWoodsPlayerController::Inventory_Select(int index)
{
	if (bPawnPossessed)
	{
		this->PossessedPlayerPawn->InventorySelect(index);
	}
}

void AThingsInWoodsPlayerController::Jump()
{
	if (bPawnPossessed)
	{
		this->PossessedPlayerPawn->HandleJump();

	} else if (bSpectatorPossessed)
	{
		//Switch spectating to next player
		AThingsInWoodsCharacter* ANewPlayerCharacter = Cast<AThingsInWoodsCharacter>(this->PossessedSpectatorPawn->GetNextPlayer());

		if (ANewPlayerCharacter != nullptr)
		{
			AThingsInWoodsHUD* HUD = Cast<AThingsInWoodsHUD>(this->GetHUD());

			if (HUD != nullptr)
			{
				HUD->PlayerPawn = ANewPlayerCharacter;
			}
		}
	}
}

void AThingsInWoodsPlayerController::StopJump()
{	
	if (bPawnPossessed)
	{
		this->PossessedPlayerPawn->HandleStopJump();
	}
}

void AThingsInWoodsPlayerController::Sprint()
{
	if (bPawnPossessed)
	{
		this->PossessedPlayerPawn->RunToggle(true);
	}
}

void AThingsInWoodsPlayerController::StopSprint()
{
	if (bPawnPossessed)
	{
		this->PossessedPlayerPawn->RunToggle(false);
	}
}

void AThingsInWoodsPlayerController::MoveForward(float fRate)
{
	if (bPawnPossessed)
	{
		this->PossessedPlayerPawn->HandleMovement(GetActorForwardVector(), fRate);
	}
}

void AThingsInWoodsPlayerController::MoveRight(float fRate)
{
	if (bPawnPossessed)
	{
		this->PossessedPlayerPawn->HandleMovement(GetActorRightVector(), fRate);
	}
}

void AThingsInWoodsPlayerController::TurnAtRate(float fRate)
{
	if (bPawnPossessed)
	{
		this->PossessedPlayerPawn->HandleTurnRotation(fRate * BaseTurnRate * GetWorld()->GetDeltaSeconds());

	} else if (bSpectatorPossessed)
	{
		this->PossessedSpectatorPawn->AddControllerYawInput(fRate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
	}
}

void AThingsInWoodsPlayerController::LookUpAtRate(float Rate)
{
	if (bPawnPossessed)
	{
		this->PossessedPlayerPawn->HandleUpRotation(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());

	} else if (bSpectatorPossessed)
	{
		this->PossessedSpectatorPawn->AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
	}
}

void AThingsInWoodsPlayerController::Use()
{
	if (bPawnPossessed)
	{
		this->PossessedPlayerPawn->HandleUseInput();
	}
}

void AThingsInWoodsPlayerController::StopUse()
{

}

void AThingsInWoodsPlayerController::Primary()
{
	if (bPawnPossessed)
	{
		this->PossessedPlayerPawn->Primary();
	}
}

void AThingsInWoodsPlayerController::StopPrimary()
{
	if (bPawnPossessed)
	{
		this->PossessedPlayerPawn->StopPrimary();
	}
}

void AThingsInWoodsPlayerController::Secondary()
{
	if (bPawnPossessed)
	{
		this->PossessedPlayerPawn->Secondary();
	}
}

void AThingsInWoodsPlayerController::StopSecondary()
{
	if (bPawnPossessed)
	{
		this->PossessedPlayerPawn->StopSecondary();
	}
}

void AThingsInWoodsPlayerController::ToggleNightVision()
{
	if (bPawnPossessed)
	{
		this->PossessedPlayerPawn->ToggleNightVision();
	}
}