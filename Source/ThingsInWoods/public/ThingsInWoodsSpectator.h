// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/SpectatorPawn.h"
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
	
	/**  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* FirstPersonCameraComponent;
private:

protected:
	/** */
	class AThingsInWoodsCharacter* AttachedPawn;

public: 
	AThingsInWoodsSpectator();
	virtual void BeginPlay();
	virtual void Tick(float DeltaSeconds) override;

	/** */
	void AttachToPlayerPawn(class AThingsInWoodsCharacter* Pawn);

	/** */
	class AThingsInWoodsCharacter* GetNextPlayer();
	
	FORCEINLINE class UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }
};