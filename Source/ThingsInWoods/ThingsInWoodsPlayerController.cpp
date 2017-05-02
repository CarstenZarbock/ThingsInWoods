// Fill out your copyright notice in the Description page of Project Settings.

#include "ThingsInWoods.h"
#include "ThingsInWoodsPlayerController.h"
#include "UnrealNetwork.h"

AThingsInWoodsPlayerController::AThingsInWoodsPlayerController()
{

}

void AThingsInWoodsPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	/* set the turn rates */
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;
}

/* ------------------------------------------
 replicated variables
*/
void AThingsInWoodsPlayerController::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AThingsInWoodsPlayerController, APlayerPawn);
	DOREPLIFETIME(AThingsInWoodsPlayerController, ASpectatorPawnOR);
}

/* ------------------------------------------
* SetPawn()
*@Param APawn* InPawn
*Super::SetPawn - Gets executed if a controller possessing a pawn
*/
void AThingsInWoodsPlayerController::SetPawn(APawn* InPawn)
{
	Super::SetPawn(InPawn);
	
	if (Cast<AThingsInWoodsCharacter>(InPawn) != nullptr)
	{
		this->SetPlayerPawn(Cast<AThingsInWoodsCharacter>(InPawn));
		this->SetSpectatorPawn(nullptr);
		return;
	}

	if (Cast<AThingsInWoodsSpectator>(InPawn) != nullptr)
	{
		this->SetSpectatorPawn(Cast<AThingsInWoodsSpectator>(InPawn));
		this->SetPlayerPawn(nullptr);
		return;
	}
}

//////////////////////////////////////////////////////////////////////////
// Input

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

//------------------------------------------------------------------------
//------------------------------------------------------------------------
// Input Interactions / Use Object related
/* ------------------------------------------
* Inventory_Drop()
* Executes the InventoryDrop function
*/
void AThingsInWoodsPlayerController::Inventory_Drop()
{
	if (this->GetPlayerPawn() != nullptr && this->GetPlayerPawn()->IsValidLowLevel())
	{
		this->GetPlayerPawn()->InventoryDrop();
	}
}

/* ------------------------------------------
* Inventory_Select()
* Executes the InventorySelect based on input key index function
*/
void AThingsInWoodsPlayerController::Inventory_Select(int index)
{
	if (this->GetPlayerPawn() != nullptr && this->GetPlayerPawn()->IsValidLowLevel())
	{
		this->GetPlayerPawn()->InventorySelect(index);
	}
}

/* ------------------------------------------
* Jump()
* Executes the Jump input, Jump if a Character is active, Select Next Player if Spectator Mode
*/
void AThingsInWoodsPlayerController::Jump()
{
	if (this->GetPlayerPawn() != nullptr && this->GetPlayerPawn()->IsValidLowLevel())
	{
		this->GetPlayerPawn()->HandleJump();
		return;
	}

	if (this->GetSpectatorPawn() != nullptr && this->GetSpectatorPawn()->IsValidLowLevel())
	{
		AThingsInWoodsCharacter* ANewPlayerCharacter = Cast<AThingsInWoodsCharacter>(this->GetSpectatorPawn()->GetNextPlayer());

		if (ANewPlayerCharacter != nullptr && ANewPlayerCharacter->IsValidLowLevel())
		{
			AThingsInWoodsHUD* HUD = Cast<AThingsInWoodsHUD>(this->GetHUD());

			if (HUD != nullptr)
			{
				HUD->PlayerPawn = ANewPlayerCharacter;
			}
		}
	}
}

/* ------------------------------------------
* StopJump()
* Executes the StopJump() when JumpKey is released
*/
void AThingsInWoodsPlayerController::StopJump()
{	
	if (this->GetPlayerPawn() != nullptr && this->GetPlayerPawn()->IsValidLowLevel())
	{
		this->GetPlayerPawn()->HandleStopJump();
	}
}

/* ------------------------------------------
* Sprint()
* Executes the Run Toggle when Sprint Input Key is pressed
*/
void AThingsInWoodsPlayerController::Sprint()
{
	if (this->GetPlayerPawn() != nullptr && this->GetPlayerPawn()->IsValidLowLevel())
	{
		this->GetPlayerPawn()->RunToggle(true);
	}
}

/* ------------------------------------------
* StopSprint()
* Executes the Run Toggle when Sprint Input Key is released
*/
void AThingsInWoodsPlayerController::StopSprint()
{
	if (this->GetPlayerPawn() != nullptr && this->GetPlayerPawn()->IsValidLowLevel())
	{
		this->GetPlayerPawn()->RunToggle(false);
	}
}

/* ------------------------------------------
* MoveForward()
*@Param float fRate - Input Rate of the Key Axis Value
*Executes Forwards / Backwards walking
*/
void AThingsInWoodsPlayerController::MoveForward(float fRate)
{
	if (this->GetPlayerPawn() != nullptr && this->GetPlayerPawn()->IsValidLowLevel())
	{
		/* Add Movement to the direction */
		this->GetPlayerPawn()->HandleMovement(GetActorForwardVector(), fRate);
	}
}

/* ------------------------------------------
* MoveRight()
*@Param float fRate - Input Rate of the Key Axis Value
*Executes Right / Left walking
*/
void AThingsInWoodsPlayerController::MoveRight(float fRate)
{
	if (this->GetPlayerPawn() != nullptr && this->GetPlayerPawn()->IsValidLowLevel())
	{
		/* Add Movement to the direction */
		this->GetPlayerPawn()->HandleMovement(GetActorRightVector(), fRate);
	}
}

/* ------------------------------------------
* TurnAtRate()
*@Param float fRate - Input Rate of the Key Axis Value
*Executes a side Turn
*/
void AThingsInWoodsPlayerController::TurnAtRate(float fRate)
{
	if (this->GetPlayerPawn() != nullptr && this->GetPlayerPawn()->IsValidLowLevel())
	{
		/* Add Rotation */
		this->GetPlayerPawn()->HandleTurnRotation(fRate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
		return;
	}

	if (this->GetSpectatorPawn() != nullptr && this->GetSpectatorPawn()->IsValidLowLevel()) //todo: Own Function?
	{
		this->GetSpectatorPawn()->AddControllerYawInput(fRate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
		return;
	}
}

/* ------------------------------------------
* LookUpAtRate()
*@Param float fRate - Input Rate of the Key Axis Value
*Executes a lookUp / lookDown turn
*/
void AThingsInWoodsPlayerController::LookUpAtRate(float Rate)
{
	if (this->GetPlayerPawn() != nullptr && this->GetPlayerPawn()->IsValidLowLevel())
	{
		/* Add Rotation */
		this->GetPlayerPawn()->HandleUpRotation(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
		return;
	}

	if (this->GetSpectatorPawn() != nullptr && this->GetSpectatorPawn()->IsValidLowLevel()) //todo: Own Function?
	{
		this->GetSpectatorPawn()->AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
		return;
	}
}

/* ------------------------------------------
* Use()
*Executes a use function if input key is pressed
*/
void AThingsInWoodsPlayerController::Use()
{
	if (this->GetPlayerPawn() != nullptr && this->GetPlayerPawn()->IsValidLowLevel())
	{
		this->GetPlayerPawn()->HandleUseInput();
	}
}

/* ------------------------------------------
* StopUse()
*Executes a stopuse function if input key is released
*/
void AThingsInWoodsPlayerController::StopUse()
{

}

/* ------------------------------------------
* Primary()
*Executes a primary function if primary key is pressed
*/
void AThingsInWoodsPlayerController::Primary()
{
	if (this->GetPlayerPawn() != nullptr && this->GetPlayerPawn()->IsValidLowLevel())
	{
		this->GetPlayerPawn()->Primary();
	}
}

/* ------------------------------------------
* StopPrimary()
*Executes a stop primary function if primary key is released
*/
void AThingsInWoodsPlayerController::StopPrimary()
{
	if (this->GetPlayerPawn() != nullptr && this->GetPlayerPawn()->IsValidLowLevel())
	{
		this->GetPlayerPawn()->StopPrimary();
	}
}

/* ------------------------------------------
* Secondary()
*Executes a secondary function if secondary key is pressed
*/
void AThingsInWoodsPlayerController::Secondary()
{
	if (this->GetPlayerPawn() != nullptr && this->GetPlayerPawn()->IsValidLowLevel())
	{
		this->GetPlayerPawn()->Secondary();
	}
}

/* ------------------------------------------
* StopSecondary()
*Executes a secondary function if secondary key is released
*/
void AThingsInWoodsPlayerController::StopSecondary()
{
	if (this->GetPlayerPawn() != nullptr && this->GetPlayerPawn()->IsValidLowLevel())
	{
		this->GetPlayerPawn()->StopSecondary();
	}
}

/* ------------------------------------------
* ToggleNightVision()
*Executes the Night Vision Toggle if Night Vision Hotkey input key is pressed
*/
void AThingsInWoodsPlayerController::ToggleNightVision()
{
	if (this->GetPlayerPawn() != nullptr && this->GetPlayerPawn()->IsValidLowLevel())
	{
		this->GetPlayerPawn()->ToggleNightVision();
	}
}
//------------------------------------------------------------------------