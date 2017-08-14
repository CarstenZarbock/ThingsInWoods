// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PlayerController.h"
#include "ThingsInWoodsPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class THINGSINWOODS_API AThingsInWoodsPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	AThingsInWoodsPlayerController();
	virtual void SetupInputComponent() override;
	virtual void BeginPlay() override;
	virtual void SetPawn(APawn* InPawn) override;

protected:
	/** */
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
		class AThingsInWoodsCharacter* PossessedPlayerPawn;
		
	/** */
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
		class AThingsInWoodsSpectator* PossessedSpectatorPawn;

	bool bPawnPossessed;
	bool bSpectatorPossessed;

public:
		/* Input */
		void Jump();
		void StopJump();
		void MoveForward(float fRate);
		void MoveRight(float fRate);
		void TurnAtRate(float fRate);
		void LookUpAtRate(float fRate);
		void Use();
		void StopUse();
		void Sprint();
		void StopSprint();
		void Primary();
		void StopPrimary();
		void Secondary();
		void StopSecondary();

		/** */
		void ToggleNightVision();

		/** Inventory Input */
		template<int index>
		void Inventory_Select()
		{
			Inventory_Select(index);
		}

		void Inventory_Select(int index);
		void Inventory_Drop();

		/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
			float BaseTurnRate;

		/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
			float BaseLookUpRate;

		/** */
		UPROPERTY(EditAnywhere, BlueprintReadWrite)
			float PickupDistance;

		/** Helper function for raycast */
		FHitResult TraceLine(FVector Start, FVector End, bool Debug);
};
