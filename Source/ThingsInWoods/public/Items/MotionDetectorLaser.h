// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BaseItem.h"
#include "MotionDetectorLaser.generated.h"

USTRUCT(BlueprintType)
struct FLaserItem
{
	GENERATED_BODY()
	/** */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		ABaseItem* LaserActor;

	/** */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		UParticleSystemComponent* ActiveParticle;

	FLaserItem()
	{

	}
};
/**
*
*/
UCLASS()
class THINGSINWOODS_API AMotionDetectorLaser : public ABaseItem
{
	GENERATED_BODY()

		AMotionDetectorLaser();
	void BeginPlay() override;
private:
protected:

public:
	/** */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		float DetectionDistance;

	/** */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		float ScanFactorTime;

	/** */
	float NextScanTime;

	/** */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		UParticleSystem* LaserParticle;

	/** */
	TArray<AActor*> RegisteredMonitor;

	/** */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TArray<FLaserItem> RegisteredLaser;

	/** */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TArray<AMotionDetectorLaser*> PassiveLaser;

	/** */
	bool isScanning;

	/** */
	bool isActive() const { return this->isScanning; };

	void CustomTick() override;
	void execPrimary() override;
	void execSecondary() override;
	void ActivateObject() override;
	void DisableObject() override;

	/** */
	void Register();

	/** */
	void UnRegister();

	/** */
	void GetAllLaserNear();

	/** */
	void RemoveFromRegisteredLaser(AActor* LaserToRemove);

	/** */
	void UnRegisterOnPassiveLaser();

	/** */
	void ClearRegisteredLaser();

	/** */
	void Scan();

	/** */
	void ExecuteScans();

	/** Helper function raycast */
	FHitResult TraceLine(FVector Start, FVector End, bool Debug, TArray<AActor*> ActorsToIgnore);
};