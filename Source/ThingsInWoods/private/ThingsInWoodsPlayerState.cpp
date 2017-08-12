// Copyright Rebound-Software

#include "ThingsInWoods.h"
#include "ThingsInWoodsPlayerState.h"

AThingsInWoodsPlayerState::AThingsInWoodsPlayerState()
{

}

AThingsInWoodsPlayerState::~AThingsInWoodsPlayerState()
{

}

void AThingsInWoodsPlayerState::SetState(EPlayerMode NewMode, bool wasAlive)
{
	this->PlayerMode = NewMode;
	this->wasAlive = wasAlive;
}
