// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Engine.h"
#include "Slender_Target_Spawn.generated.h"

UCLASS()
class THINGSINWOODS_API ASlender_Target_Spawn : public AActor
{
	GENERATED_BODY()

		UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		class USceneComponent* RootScene;
private:
protected:

public:	
	ASlender_Target_Spawn();

	virtual void BeginPlay() override;
	virtual void Tick( float DeltaSeconds ) override;
	
	/** */
	UFUNCTION(BlueprintCallable, Category = "Spawn")
		void SpawnTarget();

	/** */
	void Spawn_Activate();

	/** */
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
		bool isSpawned;

	/** */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool isCompleted;

	/** */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool PendingComplete;

	/** */
	void SetCompleted();
};
