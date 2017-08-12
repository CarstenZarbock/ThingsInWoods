// Copyright Rebound-Software

#pragma once

#include "GameFramework/PlayerState.h"
#include "ThingsInWoodsPlayerState.generated.h"

/** */
UENUM(BlueprintType)
enum class EPlayerMode : uint8
{
	PM_SPECTATOR		UMETA(DisplayName = "SPECTATOR"),
	PM_DEAD				UMETA(DisplayName = "DEAD"),
	PM_ALIVE			UMETA(DisplayName = "ALIVE"),
	PM_SAVED			UMETA(DisplayName = "SAVED")
};

/**
*
*/
UCLASS()
class THINGSINWOODS_API AThingsInWoodsPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	AThingsInWoodsPlayerState();
	~AThingsInWoodsPlayerState();

	/** */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		EPlayerMode PlayerMode;

	/** */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int	Points;

	/** */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int Blackouts;

	/** */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool wasAlive;

	/** */
	void SetState(EPlayerMode NewMode, bool wasAlive);
};



