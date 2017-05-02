// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Character.h"
#include "ThingsInWoodsCharacter.h"
#include "Slenderman.generated.h"

UCLASS()
class THINGSINWOODS_API ASlenderman : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASlenderman();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool isActive;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool isScaring;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float SpawnDistance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float SpawnDistanceRange;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float AggressiveFactor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float ScareAggressionFactor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float SpawnRate;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float ScareRange;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float KillAngle; //~40.0f
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<AThingsInWoodsCharacter*>ActivePlayers;	

	UFUNCTION(BlueprintCallable, Category = "Stats")
		void SetStats(bool Active, float AFactor);

	float NextTeleportTimer;

	void CheckForPlayerAngles(float DeltaTime);
	void GetAllActivePlayers();
	void HandleTeleport();
	void TeleportToPlayer();
};
