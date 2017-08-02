// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameStateBase.h"
#include "ThingsInWoodsGameState.generated.h"

/**
 * 
 */

 /** */
UENUM(BlueprintType)
enum class EGameType : uint8
{
	GT_RAKE		UMETA(DisplayName = "RAKE"),
	GT_SLENDER		UMETA(DisplayName = "SLENDERMAN")
};

UCLASS()
class THINGSINWOODS_API AThingsInWoodsGameState : public AGameStateBase
{
	GENERATED_BODY()
		
public:
	AThingsInWoodsGameState();

	/** */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int Current_Found_Targets;

	/** */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int Max_Found_Targets;

	/** */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		EGameType Current_GameType;
};
