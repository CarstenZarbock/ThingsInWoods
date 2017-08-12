// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BaseItem.h"
#include "Healthpack.generated.h"

/**
 * 
 */
UCLASS()
class THINGSINWOODS_API AHealthpack : public ABaseItem
{
	GENERATED_BODY()
		AHealthpack();

public:
	void execPrimary() override;
	void execSecondary() override;
};