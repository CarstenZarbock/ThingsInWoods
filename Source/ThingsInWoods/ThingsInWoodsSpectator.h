// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/SpectatorPawn.h"
#include "Engine.h"
#include "ThingsInWoodsSpectator.generated.h"

/**
 * 
 */
UCLASS()
class THINGSINWOODS_API AThingsInWoodsSpectator : public ASpectatorPawn
{
	GENERATED_BODY()
		
	/** */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* SpringarmComponent;
	
	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* FirstPersonCameraComponent;
private:

protected:
	/** */
	APawn* AAttachedPawn;

public: 
	AThingsInWoodsSpectator();
	virtual void BeginPlay();
	virtual void Tick(float DeltaSeconds) override;

	/** */
	void AttachToPlayerPawn(APawn* Pawn);

	/** */
	APawn* GetNextPlayer();
	
	FORCEINLINE class UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }
};