// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "ThingsInWoods.h"
#include "ThingsInWoodsGameMode.h"
#include "ThingsInWoodsHUD.h"
#include "ThingsInWoodsCharacter.h"
#include "ThingsInWoodsPlayerController.h"

AThingsInWoodsGameMode::AThingsInWoodsGameMode()
	: Super()
{
	// use our custom HUD class
	HUDClass = AThingsInWoodsHUD::StaticClass();
}
