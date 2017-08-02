// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BaseItem.h"
#include "BigLamp.generated.h"

/**
 *	
 */
UCLASS()
class THINGSINWOODS_API ABigLamp : public ABaseItem
{
	GENERATED_BODY()
private:
	/** */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		class USpotLightComponent* Spotlight;

public:
	ABigLamp();
		void BeginPlay() override;

	/** */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Misc)
		float Intensity;

	void execPrimary() override;
	void execSecondary() override;

	void ActivateObject() override;
	void DisableObject() override;
};