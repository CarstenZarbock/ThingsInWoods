// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "ThingsInWoods.h"
#include "ThingsInWoodsHUD.h"
#include "Engine/Canvas.h"
#include "TextureResource.h"
#include "CanvasItem.h"
#include "ThingsInWoodsCharacter.h"
#include "Engine.h"

AThingsInWoodsHUD::AThingsInWoodsHUD()
{

}

void AThingsInWoodsHUD::BeginPlay()
{
	Super::BeginPlay();
	this->PlayerPawn = Cast<AThingsInWoodsCharacter>(GWorld->GetFirstPlayerController()->GetPawn());
}


void AThingsInWoodsHUD::DrawHUD()
{
	Super::DrawHUD();
}