// Fill out your copyright notice in the Description page of Project Settings.

#include "ThingsInWoods.h"
#include "Healthpack.h"
#include "ThingsInWoodsCharacter.h"

AHealthpack::AHealthpack()
{

}

void AHealthpack::execPrimary()
{
	AThingsInWoodsCharacter* PlayerPawn = Cast<AThingsInWoodsCharacter>(this->GetOwner());
	PlayerPawn->AActionTarget->Heal(20);
}

void AHealthpack::execSecondary()
{
	AThingsInWoodsCharacter* PlayerPawn = Cast<AThingsInWoodsCharacter>(this->GetOwner());
	if (PlayerPawn != nullptr)
	{
		PlayerPawn->Heal(20);
	}
}