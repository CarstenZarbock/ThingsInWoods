// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "ThingsInWoods.h"
#include "ThingsInWoodsCharacter.h"
#include "Animation/AnimInstance.h"
#include "GameFramework/InputSettings.h"
#include "Kismet/HeadMountedDisplayFunctionLibrary.h"
#include "MotionControllerComponent.h"
#include "Engine.h"
#include "Items/Nightvision.h" //needs to be fixed
#include "UnrealNetwork.h"
#include "ThingsInWoodsPlayerState.h"
#include "ThingsInWoodsSpectator.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

AThingsInWoodsCharacter::AThingsInWoodsCharacter()
{
	/* We setup the collision capsule */
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
	GetMesh()->SetOwnerNoSee(true);

	/* Create the first person camera component */
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->RelativeLocation = FVector(-39.56f, 1.75f, 64.f); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	/* Setup the FIRST person hands mesh | third person character mesh is already setup by APawn:: */
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Hands Mesh"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->RelativeRotation = FRotator(1.9f, -19.19f, 5.2f);
	Mesh1P->RelativeLocation = FVector(-0.5f, -4.4f, -155.7f);

	/* Create a scene component to use as item position in first person */
	FP_Gun = CreateDefaultSubobject<USceneComponent>(TEXT("FP_Gun"));
	FP_Gun->SetupAttachment(Mesh1P); // Attach it to the hands model

	/* Create a scene component to use as item position in third person */
	TP_Gun = CreateDefaultSubobject<USceneComponent>(TEXT("TP_Gun"));
	TP_Gun->SetupAttachment(this->GetMesh()); // Attach it to the third person model

	/* Create inventory slots for third person */
	TP_Mesh_Inv1 = CreateDefaultSubobject<USceneComponent>(TEXT("TP_InventorySlot_1"));
	TP_Mesh_Inv1->SetupAttachment(this->GetMesh());

	TP_Mesh_Inv2 = CreateDefaultSubobject<USceneComponent>(TEXT("TP_InventorySlot_2"));
	TP_Mesh_Inv2->SetupAttachment(this->GetMesh());

	TP_Mesh_Inv3 = CreateDefaultSubobject<USceneComponent>(TEXT("TP_InventorySlot_3"));
	TP_Mesh_Inv3->SetupAttachment(this->GetMesh());

	TP_Mesh_Inv4 = CreateDefaultSubobject<USceneComponent>(TEXT("TP_InventorySlot_4"));
	TP_Mesh_Inv4->SetupAttachment(this->GetMesh());

	TP_Mesh_Inv5 = CreateDefaultSubobject<USceneComponent>(TEXT("TP_InventorySlot_5"));
	TP_Mesh_Inv5->SetupAttachment(this->GetMesh());

	/* Setup audio components */
	VoiceAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("Voice Audio"));
	VoiceAudioComponent->SetupAttachment(this->GetMesh());

	FootAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("Foot Audio"));
	FootAudioComponent->SetupAttachment(this->GetMesh());

	VarAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("Var Audio"));
	VarAudioComponent->SetupAttachment(this->GetMesh());

	/* Init the inventory */
	this->Inventory.Empty();
	this->Inventory.AddDefaulted(5); // Inventory Max Size

	for (int i = 0; i < 5; i++)
	{
		this->Inventory[i].Item.Empty();
	}
}

void AThingsInWoodsCharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AThingsInWoodsCharacter, iStamina);
	DOREPLIFETIME(AThingsInWoodsCharacter, fHealth);
	DOREPLIFETIME(AThingsInWoodsCharacter, bIsAlive);
	DOREPLIFETIME(AThingsInWoodsCharacter, bIsRunning);
	DOREPLIFETIME(AThingsInWoodsCharacter, bIsUsingItem);
	DOREPLIFETIME(AThingsInWoodsCharacter, bIsUsingPrimary);
	DOREPLIFETIME(AThingsInWoodsCharacter, bIsUsingOverTime);
	DOREPLIFETIME(AThingsInWoodsCharacter, fUsingPercentage);
	DOREPLIFETIME(AThingsInWoodsCharacter, bIsBlackout);
	DOREPLIFETIME(AThingsInWoodsCharacter, fBlackoutPercentage);
	DOREPLIFETIME(AThingsInWoodsCharacter, fBlackoutNextHitTime); //todo: check
	DOREPLIFETIME(AThingsInWoodsCharacter, fScare_Current); //todo: check
	DOREPLIFETIME(AThingsInWoodsCharacter, iInventorySelectedObject);
	DOREPLIFETIME(AThingsInWoodsCharacter, Inventory);
}

void AThingsInWoodsCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	/* There's no skeleton created in the constructor, so it has to be attached here. */
	FP_Gun->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::KeepRelative, true), TEXT("GripPoint"));
	TP_Gun->AttachToComponent(this->GetMesh(), FAttachmentTransformRules(EAttachmentRule::KeepRelative, true), TEXT("RightHand"));
	TP_Mesh_Inv1->AttachToComponent(this->GetMesh(), FAttachmentTransformRules(EAttachmentRule::KeepRelative, true), TEXT("Spine2"));
	TP_Mesh_Inv2->AttachToComponent(this->GetMesh(), FAttachmentTransformRules(EAttachmentRule::KeepRelative, true), TEXT("RightUpLeg"));
	TP_Mesh_Inv3->AttachToComponent(this->GetMesh(), FAttachmentTransformRules(EAttachmentRule::KeepRelative, true), TEXT("LeftUpLeg"));
	TP_Mesh_Inv4->AttachToComponent(this->GetMesh(), FAttachmentTransformRules(EAttachmentRule::KeepRelative, true), TEXT("RightShoulder"));
	TP_Mesh_Inv5->AttachToComponent(this->GetMesh(), FAttachmentTransformRules(EAttachmentRule::KeepRelative, true), TEXT("LeftShoulder"));

	/* We hide the hands mesh - as long as it's the default one */
	//Mesh1P->SetHiddenInGame(true, false);

	this->iInventorySelectedObject = -1;
	this->fHealth = this->fHealthMax;
	this->bIsAlive = true;
	this->iStamina = this->iStaminaMax;
	this->fNextStaminaHitTime = 0;
	this->bIsRunning = false;

	/* We register our characters to the other players */
	this->SayHallo();
}


void AThingsInWoodsCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (this->bIsAlive)
	{
		this->BlackoutHandle();
		this->ItemUsageHandle();
		this->StaminaHandle();

		this->HandleScare(DeltaTime);
	}
}


void AThingsInWoodsCharacter::HandleUseInput()
{
	if (this->bIsAlive && !this->bIsBlackout)
	{
		//Re-initialize hit info
		FHitResult RV_Hit(ForceInit);
		RV_Hit = TraceLine((this->GetFirstPersonCameraComponent()->GetForwardVector() * 100) + this->GetFirstPersonCameraComponent()->GetComponentLocation(), (this->GetFirstPersonCameraComponent()->GetForwardVector() * this->fPickupDistance) + this->GetFirstPersonCameraComponent()->GetComponentLocation(), true);

		if (RV_Hit.GetActor() != NULL)
		{
			ABaseItem* HitItem = Cast<ABaseItem>(RV_Hit.GetActor());
			if (HitItem != nullptr && HitItem->IsValidLowLevel())
			{
				switch (HitItem->ItemType)
				{
				case EItemType::IT_PICKUP:
					if (!HitItem->isOwned)
					{
						this->PickupObject(HitItem);
					}
					break;
				case EItemType::IT_USAGE:
					this->UseUsageObject(HitItem);
					break;
				default:
					break;
				}
			}
		}
	}
}

void AThingsInWoodsCharacter::HandleMovement(FVector WorldDirection, float ScaleValue)
{
	if (this->bIsAlive && !this->bIsBlackout && this->CanMove())
	{
		this->AddMovementInput(WorldDirection, ScaleValue);

		if (this->GetMovementComponent()->IsMovingOnGround())
		{
			this->PlaySound_Steps();
		}
	}
}

void AThingsInWoodsCharacter::HandleTurnRotation(float fRate)
{
	if (this->bIsAlive && !this->bIsBlackout)
	{
		this->AddControllerYawInput(fRate);
	}
}

void AThingsInWoodsCharacter::HandleUpRotation(float fRate)
{
	if (this->bIsAlive && !this->bIsBlackout)
	{
		this->AddControllerPitchInput(fRate);
	}
}

void AThingsInWoodsCharacter::Landed(const FHitResult & Hit)
{
	Super::Landed(Hit);

	FHitResult GroundHit = this->GetGroundHit();
	if (GroundHit.bBlockingHit && GroundHit.PhysMaterial->IsValidLowLevel())
	{
		this->PlaySound(this->GetSound_Landed(GroundHit.PhysMaterial->SurfaceType), this->FootAudioComponent, false);
	}

}

void AThingsInWoodsCharacter::HandleJump()
{
	if (this->bIsAlive && !this->bIsBlackout && this->CanMove())
	{
		if (this->GetMovementComponent()->IsMovingOnGround())
		{
			FHitResult GroundHit = this->GetGroundHit();

			if (GroundHit.bBlockingHit && GroundHit.PhysMaterial->IsValidLowLevel())
			{
				this->PlaySound(this->GetSound_Jump(GroundHit.PhysMaterial->SurfaceType), this->FootAudioComponent, false);
			}

			this->Jump();
		}
	}
}

void AThingsInWoodsCharacter::HandleStopJump()
{
	if (this->bIsAlive && !this->bIsBlackout)
	{
		this->StopJumping();
	}
}

bool AThingsInWoodsCharacter::CanMove()
{
	if (this->bIsUsingItem && this->bIsUsingOverTime)
	{
		return false;
	}

	return true;
}

void AThingsInWoodsCharacter::SetMaximumWalkSpeed(float fNewMaximumWalkSpeed)
{
	this->GetCharacterMovement()->MaxWalkSpeed = fNewMaximumWalkSpeed;
}

void AThingsInWoodsCharacter::execRunToggle(bool bNewIsRunning)
{
	if (this->bIsRunning && this->iStamina >= this->fMinimumStaminaToRun)
	{
		/* slow down if injured */
		if (this->IsInjured())
		{
			this->SetMaximumWalkSpeed(this->fMaxWalkSpeedRunningInjured);
		}
		else
		{
			this->SetMaximumWalkSpeed(this->fMaxWalkSpeedRunning);
		}

		this->bIsRunning = true;
	}
	else /* character is not running or too low on stamina */
	{
		if (this->IsInjured())
		{
			this->SetMaximumWalkSpeed(this->fMaxWalkSpeedInjured);
		}
		else
		{
			this->SetMaximumWalkSpeed(this->fMaxWalkSpeed);
		}

		this->bIsRunning = false;
	}
}

bool AThingsInWoodsCharacter::RunToggle_Validate(bool bNewIsRunning)
{
	return true;
}

void AThingsInWoodsCharacter::RunToggle_Implementation(bool bNewIsRunning)
{
	if (this->bIsAlive && !this->bIsBlackout && this->CanMove())
	{
		/* executes local */
		this->execRunToggle(bNewIsRunning);

		/* if we're a client, run it serverside */
		if (Role < ROLE_Authority)
		{
			this->Server_RunToggle(bNewIsRunning);
		}
	}
}

bool AThingsInWoodsCharacter::Server_RunToggle_Validate(bool bNewIsRunning)
{
	return true;
}

void AThingsInWoodsCharacter::Server_RunToggle_Implementation(bool bNewIsRunning)
{
	/* Character movement is serverside, no need to replicate to other clients */
	this->RunToggle(bNewIsRunning); //todo: Note: could end in endless loop, if: change to execRunToggle
}

void AThingsInWoodsCharacter::StaminaHandle()
{
	/* replace time with delta time timer */
	if (GetGameTimeSinceCreation() >= this->fNextStaminaHitTime)
	{
		if (this->bIsRunning)
		{
			this->iStamina -= this->fStaminaReducementFactor;
			if (this->iStamina <= 0)
			{
				/* Stamina is 0 or below, disable running */
				this->iStamina = 0;
				this->RunToggle(false);
			}

			/* Set the next "Hit" time based on the reducement timer */
			this->fNextStaminaHitTime = GetGameTimeSinceCreation() + this->fStaminaHitTime;
		}
		else /* while walking / standing gain stamina */
		{
			this->iStamina += this->fStaminaGainTime;
			if (this->iStamina > this->iStaminaMax)
			{
				this->iStamina = this->iStamina;
			}

			/* Set the next "Hit" time based on the gain timer */
			this->fNextStaminaHitTime= GetGameTimeSinceCreation() + this->fStaminaGainTime;
		}
	}
}

void AThingsInWoodsCharacter::execPlaySound(USoundBase* Sound, UAudioComponent* AudioComponent, bool ifAvailable)
{
	if (AudioComponent->IsValidLowLevel() && Sound != nullptr && Sound->IsValidLowLevel())
	{
		/* Audio Component is busy, cancel new sound play */
		if (ifAvailable && AudioComponent->IsPlaying())
		{
			return;
		}

		AudioComponent->SetSound(Sound);
		AudioComponent->Play();
	}
}

bool AThingsInWoodsCharacter::PlaySound_Validate(USoundBase* Sound, UAudioComponent* AudioComponent, bool ifAvailable)
{
	return true;
}

void AThingsInWoodsCharacter::PlaySound_Implementation(USoundBase* Sound, UAudioComponent* AudioComponent, bool ifAvailable)
{
	if (!HasAuthority())
	{
		/* execute clientside first */
		this->execPlaySound(Sound, AudioComponent, ifAvailable);

		/* replicate to others and play server side*/
		this->Server_PlaySound(Sound, AudioComponent, ifAvailable);
	}
	else /* Server */
	{
		/* replicate to others and play server side */
		this->Server_PlaySound(Sound, AudioComponent, ifAvailable);
	}
}

bool AThingsInWoodsCharacter::Server_PlaySound_Validate(USoundBase* Sound, UAudioComponent* AudioComponent, bool ifAvailable)
{
	return true;
}

void AThingsInWoodsCharacter::Server_PlaySound_Implementation(USoundBase* Sound, UAudioComponent* AudioComponent, bool ifAvailable)
{
	/* execute serverside first */
	this->execPlaySound(Sound, AudioComponent, ifAvailable);

	/* Server to Clients replication */
	this->Multi_PlaySound(Sound, AudioComponent, ifAvailable);
}

bool AThingsInWoodsCharacter::Multi_PlaySound_Validate(USoundBase* Sound, UAudioComponent* AudioComponent, bool ifAvailable)
{
	return true;
}

void AThingsInWoodsCharacter::Multi_PlaySound_Implementation(USoundBase* Sound, UAudioComponent* AudioComponent, bool ifAvailable)
{
	/* Only execute on other clients, not on owning client / server */
	/* Controller does only exist locally (owning client and server), so return if it does */
	if (this->GetController() != nullptr)
	{
		return;
	}

	/* executes sound on client */
	this->execPlaySound(Sound, AudioComponent, ifAvailable);
}

FGroundSounds* AThingsInWoodsCharacter::GetGroundSoundStack(EPhysicalSurface PhysSurface)
{
	switch (PhysSurface)
	{
	case SurfaceType_Default:
		return this->FSounds_Ground.GetDefault();
		break;
	case SurfaceType1: //Grass
		return this->FSounds_Ground.GetGrass();
		break;
	case SurfaceType2: //Dirt
		return this->FSounds_Ground.GetDirt();
		break;
	case SurfaceType3: //Metal
		return this->FSounds_Ground.GetMetal();
		break;
	case SurfaceType4: //Wood
		return this->FSounds_Ground.GetWood();
		break;
	case SurfaceType5: //Stone
		return this->FSounds_Ground.GetStone();
		break;
	case SurfaceType6: //Water
		return this->FSounds_Ground.GetWater();
		break;
	default:
		break;
	}

	return this->FSounds_Ground.GetDefault();
}

USoundBase* AThingsInWoodsCharacter::GetSound_Jump(EPhysicalSurface PhysSurface)
{
	FGroundSounds* SoundStack = this->GetGroundSoundStack(PhysSurface);
	
	return SoundStack->Jump_Start;
}

USoundBase* AThingsInWoodsCharacter::GetSound_Landed(EPhysicalSurface PhysSurface)
{
	FGroundSounds* SoundStack = this->GetGroundSoundStack(PhysSurface);

	return SoundStack->Jump_Landing;
}

USoundBase* AThingsInWoodsCharacter::GetSound_Step(EPhysicalSurface PhysSurface)
{
	FGroundSounds* SoundStack = this->GetGroundSoundStack(PhysSurface);

	return SoundStack->Step;
}

USoundBase* AThingsInWoodsCharacter::GetSound_Step_Run(EPhysicalSurface PhysSurface)
{
	FGroundSounds* SoundStack = this->GetGroundSoundStack(PhysSurface);

	return SoundStack->Step_Run;
}

void AThingsInWoodsCharacter::PlaySound_Steps()
{
	FHitResult GroundHit = this->GetGroundHit();

	if (GroundHit.bBlockingHit && GroundHit.PhysMaterial->IsValidLowLevel())
	{
		USoundBase* StepSound = nullptr;

		/* play different sound on running / walking */
		if (!this->bIsRunning)
		{
			StepSound = this->GetSound_Step(GroundHit.PhysMaterial->SurfaceType);
		}
		else
		{
			StepSound = this->GetSound_Step_Run(GroundHit.PhysMaterial->SurfaceType);
		}

		/* Play replicated sound on audio component */
		this->PlaySound(StepSound, this->FootAudioComponent, true); //todo: check for network usage!
	}
}



bool AThingsInWoodsCharacter::IsInjured()
{
	if ((100 / this->fHealthMax * this->fHealth) <= 50)
	{
		return true;
	}

	return false;
}

void AThingsInWoodsCharacter::Heal(unsigned int iHealAmount)
{
	this->fHealth += iHealAmount;

	if (this->fHealth > this->fHealthMax)
	{
		this->fHealth = this->fHealthMax;
	}

	if (this->bIsBlackout)
	{
		this->bIsBlackout = false; //todo: Add Toggle Blackout?
	}
}

void AThingsInWoodsCharacter::execApplyDamage(float fDamageAmount)
{
	this->fHealth -= fDamageAmount;

	this->PlaySound(this->Pain, this->VoiceAudioComponent, true);

	if (this->fHealth <= 0.0f)
	{
		this->fHealth = 0.0f;

		this->EnableBlackout();
	}
}

bool AThingsInWoodsCharacter::ApplyDamage_Validate(float fDamageAmount)
{
	return true;
}

void AThingsInWoodsCharacter::ApplyDamage_Implementation(float fDamageAmount)
{
	/* Runs only on server */
	if (HasAuthority())
	{
		this->Server_ApplyDamage(fDamageAmount);
	}
}

bool AThingsInWoodsCharacter::Server_ApplyDamage_Validate(float fDamageAmount)
{
	return true;
}

void AThingsInWoodsCharacter::Server_ApplyDamage_Implementation(float fDamageAmount)
{
	/* Execute damage serverside */
	this->execApplyDamage(fDamageAmount);
}


bool AThingsInWoodsCharacter::Server_DeathHandle_Validate()
{
	return true;
}

void AThingsInWoodsCharacter::Server_DeathHandle_Implementation()
{
	/* Mark player as dead in the player state */
	AThingsInWoodsPlayerState* PS = Cast<AThingsInWoodsPlayerState>(this->PlayerState);
	if (PS != nullptr)
	{
		PS->PlayerMode = EPlayerMode::PM_DEAD;
	}

	/* spawn a spectator pawn and possess it */
	AThingsInWoodsSpectator* SpecPawn = GetWorld()->SpawnActor<AThingsInWoodsSpectator>();
	APlayerController* PC = Cast<APlayerController>(this->GetController());
	if (PC != nullptr)
	{
		PC->Possess(SpecPawn);
	}
}

void AThingsInWoodsCharacter::execSayHallo(AThingsInWoodsCharacter* PlayerCharacter)
{
	this->ActivePlayers.AddUnique(PlayerCharacter);
}

bool AThingsInWoodsCharacter::SayHallo_Validate()
{
	return true;
}

void AThingsInWoodsCharacter::SayHallo_Implementation()
{
	/* only server registers new characters */
	if (HasAuthority())
	{
		this->Server_SayHallo();
	}
}

bool AThingsInWoodsCharacter::Server_SayHallo_Validate()
{
	return true;
}

void AThingsInWoodsCharacter::Server_SayHallo_Implementation()
{
	/* get all other characters and register this character on them */
	//todo: replace with PawnIterator instead ActorIterator
	for (TActorIterator<AThingsInWoodsCharacter> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		AThingsInWoodsCharacter* OtherPlayerCharacter = *ActorItr;

		/* only if the character is valid and isn't this one*/
		if (OtherPlayerCharacter != nullptr && OtherPlayerCharacter != this)
		{
			/* send register */
			OtherPlayerCharacter->Multi_SayHallo(this);
		}
	}
}

bool AThingsInWoodsCharacter::Multi_SayHallo_Validate(AThingsInWoodsCharacter* PlayerCharacter)
{
	return true;
}

void AThingsInWoodsCharacter::Multi_SayHallo_Implementation(AThingsInWoodsCharacter* PlayerCharacter)
{
	/* Executed on client */
	this->execSayHallo(PlayerCharacter);
}


FHitResult AThingsInWoodsCharacter::GetGroundHit()
{
	FVector EndVec = this->GetActorLocation() + (this->GetActorUpVector() * -100); //todo: replace -100 with capsule halfsize + buffer
	FHitResult GroundHit = this->TraceLine(this->GetActorLocation(), EndVec, false);
	return GroundHit;
}

FHitResult AThingsInWoodsCharacter::TraceLine(FVector Start, FVector End, bool Debug)
{
	//todo: rework function
	FCollisionQueryParams RV_TraceParams = FCollisionQueryParams(FName(TEXT("RV_Trace")), true, this);
	RV_TraceParams.bTraceComplex = false;
	RV_TraceParams.bTraceAsyncScene = true;
	RV_TraceParams.bReturnPhysicalMaterial = true;

	if (Debug)
	{
		const FName TraceTag("TraceTag");
		GetWorld()->DebugDrawTraceTag = TraceTag;
		RV_TraceParams.TraceTag = TraceTag;
	}

	//Re-initialize hit info
	FHitResult RV_Hit(ForceInit);

	GetWorld()->LineTraceSingleByChannel(
		RV_Hit,		       //result
		Start,			  //start
		End,			 //end
		ECC_Visibility, //collision channel
		RV_TraceParams
	);

	if (Debug)
	{
		DrawDebugLine(
			GetWorld(),
			Start,
			RV_Hit.ImpactPoint,
			FColor(255, 0, 0),
			false, -1, 0,
			12.333
		);
	}

	return RV_Hit;
}

void AThingsInWoodsCharacter::execBlackoutHandle()
{
	if (this->bIsBlackout)
	{
		/* todo: replace with delta timer */
		if (GetGameTimeSinceCreation() >= this->fBlackoutNextHitTime)
		{
			this->fBlackoutPercentage -= this->fBlackoutFactor;

			this->fBlackoutNextHitTime = GetGameTimeSinceCreation() + this->fBlackoutHitTimeInSeconds;

			if (this->fBlackoutPercentage <= 0.0f)
			{
				this->bIsAlive = false;
				this->Server_DeathHandle();
			}
		}
	}
}

bool AThingsInWoodsCharacter::BlackoutHandle_Validate()
{
	return true;
}

void AThingsInWoodsCharacter::BlackoutHandle_Implementation()
{
	/* only run serverside */
	if (HasAuthority())
	{
		this->Server_BlackoutHandle();
	}
}

bool AThingsInWoodsCharacter::Server_BlackoutHandle_Validate()
{
	return true;
}

void AThingsInWoodsCharacter::Server_BlackoutHandle_Implementation()
{
	this->execBlackoutHandle();
}

void AThingsInWoodsCharacter::execEnableBlackout()
{
	if (!this->bIsBlackout)
	{
		/* active blackout and init blackout settings */
		this->bIsBlackout = true;
		this->fBlackoutPercentage = 100.0f;
		this->fBlackoutNextHitTime = GetGameTimeSinceCreation() + this->fBlackoutHitTimeInSeconds;

		/* only play sound clientside, do not replicate */
		this->execPlaySound(this->Death, this->VoiceAudioComponent, false);
	}
}

bool AThingsInWoodsCharacter::EnableBlackout_Validate()
{
	return true;
}

void AThingsInWoodsCharacter::EnableBlackout_Implementation()
{
	/* If owning client is client, not server */
	if (!HasAuthority())
	{
		/* enable on owning client */
		this->execEnableBlackout();

		/* call server to enable */
		this->Server_EnableBlackout();
	}
	else
	{
		/* executes on server and replicates to all clients except owning client */
		this->Server_EnableBlackout();
	}
}

bool AThingsInWoodsCharacter::Server_EnableBlackout_Validate()
{
	return true;
}

void AThingsInWoodsCharacter::Server_EnableBlackout_Implementation()
{
	/* run on server */
	this->execEnableBlackout();

	/* cast to everybody else except owning client and server */
	this->Multi_EnableBlackout();
}

bool AThingsInWoodsCharacter::Multi_EnableBlackout_Validate()
{
	return true;
}

void AThingsInWoodsCharacter::Multi_EnableBlackout_Implementation()
{
	/* Only execute on other clients, not on owning client / server */
	/* Controller does only exist locally (owning client and server), so return if it does */
	if (this->GetController() != nullptr)
	{
		return;
	}

	this->execEnableBlackout();
}

void AThingsInWoodsCharacter::AddItemToInventory(int iInventorySlot, ABaseItem* Object)
{
	this->Inventory[iInventorySlot].Item.Add(Object);

	/* Set Item Actor to picked up */
	Object->SetOwner(this);
	Object->SetState(EObjectState::OS_STACKED); //todo: fix object states
}

void AThingsInWoodsCharacter::execPickupObject(ABaseItem* PickUpObject)
{
	/* Check if the Item is stackable */
	int iInventorySize = this->Inventory.Num();

	for (int i = 0; i < iInventorySize; i++)
	{
		/* Current Amount of Items stacked in Slot */
		int iItemSlotSize = this->Inventory[i].Item.Num();

		/* Itemslot contains items */
		if (iItemSlotSize > 0)
		{
			/* Get first item of Itemslot */
			ABaseItem* InventorySlotItem = this->Inventory[i].Item[0];

			/* Same object class as the pickup one */
			if (InventorySlotItem->IsValidLowLevel() && InventorySlotItem->GetClass() == PickUpObject->GetClass())
			{
				/* Stackable and Stack Amount is below stack amount limit */
				if (InventorySlotItem->bIsStackable && iItemSlotSize < InventorySlotItem->iStackSize)
				{
					/* Add Item to Inventory */
					this->AddItemToInventory(i, PickUpObject);

					/* select item if player does not have an active item */
					if (this->iInventorySelectedObject == -1)
					{
						this->InventorySelect(i);
					}

					/* finished */
					return;
				}
			}
		}
	}

	/* Item is not stackable, check if we can pick it up */
	for (int i = 0; i < iInventorySize; i++)
	{
		if (this->Inventory[i].Item.Num() <= 0)
		{
			/* Add Item to Inventory */
			this->AddItemToInventory(i, PickUpObject);

			/* select item if player character has not an active item */
			if (this->iInventorySelectedObject == -1)
			{
				this->InventorySelect(i);
			}

			/* finished */
			return;
		}
	}

	/* inventory is full, return */
	return;
}

bool AThingsInWoodsCharacter::PickupObject_Validate(ABaseItem* PickUpObject)
{
	return true;
}

void AThingsInWoodsCharacter::PickupObject_Implementation(ABaseItem* PickUpObject)
{
	/* client */
	if (!HasAuthority())
	{
		/* execute clientside first */
		this->execPickupObject(PickUpObject);

		/* replicate to server */
		this->Server_PickupObject(PickUpObject);
	}
	else /* server */
	{
		/* executes on server and replicates to other client except owning client */
		this->Server_PickupObject(PickUpObject);
	}
}

bool AThingsInWoodsCharacter::Server_PickupObject_Validate(ABaseItem* PickUpObject)
{
	return true;
}

void AThingsInWoodsCharacter::Server_PickupObject_Implementation(ABaseItem* PickUpObject)
{
	/* execute serverside first */
	this->execPickupObject(PickUpObject);

	/* replicates to all other clients except owning client */
	this->Multi_PickupObject(PickUpObject);
}

bool AThingsInWoodsCharacter::Multi_PickupObject_Validate(ABaseItem* PickUpObject)
{
	return true;
}

void AThingsInWoodsCharacter::Multi_PickupObject_Implementation(ABaseItem* PickUpObject)
{
	/* Only execute on other clients, not on owning client / server */
	/* Controller does only exist locally (owning client and server), so return if it does */
	if (this->GetController() != nullptr)
	{
		return;
	}

	this->execPickupObject(PickUpObject);
}


void AThingsInWoodsCharacter::execInventoryDrop()
{
	/* Check if an item is currently selected */
	int InventoryIndex = this->iInventorySelectedObject;
	if (InventoryIndex == -1) { return; }

	/* Drop Object */
	/* Get The world location to drop */
	// Item Position + Camera Forwardvector * 100 - 100 Units infront of player
	FVector DropLocation = this->FP_Gun->GetComponentLocation() + (this->FirstPersonCameraComponent->GetForwardVector() * 100); //todo: replace with traceline?
	

	TArray<ABaseItem*>* InventoryStack = &this->Inventory[InventoryIndex].Item; //todo: check

	int iInventoryItemStackSize = InventoryStack->Num();

	if (iInventoryItemStackSize > 0)
	{
		ABaseItem* InventoryItem = (*InventoryStack)[0];

		if (InventoryItem->IsValidLowLevel())
		{
			/* Detach from player character */
			InventoryItem->DetachRootComponentFromParent(); //todo: change :^)

			/* Set State & Drop */
			InventoryItem->SetState(EObjectState::OS_WORLD, DropLocation); //todo: change :^)

			/* remove from inventory */
			/* is stackable? */
			if (InventoryItem->bIsStackable)
			{
				/* only remove one instance of the item */
				InventoryStack->RemoveSingle(InventoryItem);

				/* if item stack is empty, clear to be sure, deselect / select next item */
				if (InventoryStack->Num() <= 0)
				{
					InventoryStack->Empty();
					this->InventoryDeselect();
					this->InventorySelect(-1); //todo: select next item in inventory if possible, instead nothing
					return;
				}
				else
				{
					/* item stack is not empty, select next one in stock */
					//todo: add selection
				}
			}
			else /* not stackable, clear item stack, select nothing / next item */
			{
				InventoryStack->Empty();
				this->InventoryDeselect();
				this->InventorySelect(-1);
				return;
			}
		}
	}
	else
	{
		//Client / Server complication, no item to drop
	}
}

bool AThingsInWoodsCharacter::InventoryDrop_Validate()
{
	return true;
}

void AThingsInWoodsCharacter::InventoryDrop_Implementation()
{
	if (this->bIsAlive && !this->bIsBlackout)
	{
		//(Role > ROLE_Authority
		/* owning client */
		if (!HasAuthority())
		{
			/* execute clientside */
			this->execInventoryDrop();
			/* send to server */
			this->Server_InventoryDrop();
		}
		else /* server */
		{
			/* executes on server and replicates to other clients except owning client */
			this->Server_InventoryDrop();
		}
	}
}

bool AThingsInWoodsCharacter::Server_InventoryDrop_Validate()
{
	return true;
}

void AThingsInWoodsCharacter::Server_InventoryDrop_Implementation()
{
	/* execute serverside */
	this->execInventoryDrop();

	/* replicates to all others */
	this->Multi_InventoryDrop();
}

bool AThingsInWoodsCharacter::Multi_InventoryDrop_Validate()
{
	return true;
}

void AThingsInWoodsCharacter::Multi_InventoryDrop_Implementation()
{
	/* Only execute on other clients, not on owning client / server */
	/* Controller does only exist locally (owning client and server), so return if it does */
	if (this->GetController() != nullptr)
	{
		return;
	}

	this->execInventoryDrop();
}

void AThingsInWoodsCharacter::execInventorySelect(int iInventoryIndex)
{
	if (iInventoryIndex != -1)
	{
		TArray<ABaseItem*>* InventoryStack = &this->Inventory[iInventoryIndex].Item;

		/* be sure the inventory stack is not empty, and it's not already selected */
		if (InventoryStack->Num() > 0 && this->iInventorySelectedObject != iInventoryIndex)
		{
			/* Select the new item - first item of the stack */
			ABaseItem* TargetItem = (*InventoryStack)[0];
			if (TargetItem->IsValidLowLevel())
			{
				this->iInventorySelectedObject = iInventoryIndex;

				/* Attach actor to FP_Gun Demo Scene and apply visibility settings */
				TargetItem->AttachToComponent(this->FP_Gun, FAttachmentTransformRules::SnapToTargetIncludingScale);
				//todo: Third person different attachement than first person, disable visibility settings

				TargetItem->GetMesh()->SetOwnerNoSee(false);
				TargetItem->GetMesh()->SetOnlyOwnerSee(true);
				/* set new item state */
				TargetItem->SetState(EObjectState::OS_INVENTORY);

				/* set current carry animation type */
				this->ECurrentAnimationType = TargetItem->EAnimationTypeCurrent;
			}
		}
	}
	else
	{
		/* if inventory index is -1 select the next available item */
		// todo: Add a seperate function to select the next item
		for (int i = 0; i < this->Inventory.Num(); i++)
		{
			if (this->Inventory[i].Item.Num() > 0)
			{
				this->InventorySelect(i);
				return;
			}
		}
	}
}

bool AThingsInWoodsCharacter::InventorySelect_Validate(int iInventoryIndex)
{
	return true;
}

void AThingsInWoodsCharacter::InventorySelect_Implementation(int iInventoryIndex)
{
	if (this->bIsAlive && !this->bIsBlackout)
	{
		/* client */
		if (!HasAuthority())
		{
			/* execute clientside first */
			this->execInventorySelect(iInventoryIndex);

			/* replicate to server */
			this->Server_InventorySelect(iInventoryIndex);
		}
		else /* server */
		{
			/* execute on server and replicate to all others except owning client */
			this->Server_InventorySelect(iInventoryIndex);
		}
	}
}

bool AThingsInWoodsCharacter::Server_InventorySelect_Validate(int iInventoryIndex)
{
	return true;
}

void AThingsInWoodsCharacter::Server_InventorySelect_Implementation(int iInventoryIndex)
{
	/* execute on server */
	this->execInventorySelect(iInventoryIndex);

	/* replicate to all other clients except owning */
	this->Multi_InventorySelect(iInventoryIndex);
}

bool AThingsInWoodsCharacter::Multi_InventorySelect_Validate(int iInventoryIndex)
{
	return true;
}

void AThingsInWoodsCharacter::Multi_InventorySelect_Implementation(int iInventoryIndex)
{
	/* Only execute on other clients, not on owning client / server */
	/* Controller does only exist locally (owning client and server), so return if it does */
	if (this->GetController() != nullptr)
	{
		return;
	}

	this->execInventorySelect(iInventoryIndex);
}

void AThingsInWoodsCharacter::execInventoryDeselect()
{
	/* sets selected inventory stack to unused and resets animation */
	this->iInventorySelectedObject = -1;
	this->ECurrentAnimationType = EAnimationType::AS_NORMAL;
}

bool AThingsInWoodsCharacter::InventoryDeselect_Validate()
{
	return true;
}

void AThingsInWoodsCharacter::InventoryDeselect_Implementation()
{
	/* client */
	if (!HasAuthority())
	{
		/* execute clientside first */
		this->execInventoryDeselect();

		/* replicate to server */
		this->Server_InventoryDeselect();
	}
	else /* server */
	{
		/* execute on server and replicate to all clients except owning client */
		this->Server_InventoryDeselect();
	}
}

bool AThingsInWoodsCharacter::Server_InventoryDeselect_Validate()
{
	return true;
}

void AThingsInWoodsCharacter::Server_InventoryDeselect_Implementation()
{
	/* execute on server */
	this->execInventoryDeselect();

	/* replicates to all clients except owning client */
	this->Multi_InventoryDeselect();
}

bool AThingsInWoodsCharacter::Multi_InventoryDeselect_Validate()
{
	return true;
}

void AThingsInWoodsCharacter::Multi_InventoryDeselect_Implementation()
{
	/* Only execute on other clients, not on owning client / server */
	/* Controller does only exist locally (owning client and server), so return if it does */
	if (this->GetController() != nullptr)
	{
		return;
	}
	this->execInventoryDeselect();
}

void AThingsInWoodsCharacter::execPrimary()
{
	int iInventoryStackIndex = this->iInventorySelectedObject;
	if (iInventoryStackIndex != -1)
	{
		TArray<ABaseItem*>* InventoryStack = &this->Inventory[iInventoryStackIndex].Item;
		
		if (InventoryStack->Num() > 0)
		{
			ABaseItem* TargetItem = (*InventoryStack)[0];
			if (TargetItem->IsValidLowLevel())
			{
				bool bIsUsable = false;
				FHitResult RV_Hit(ForceInit);
				float TargetDistance;
				FUsage* TargetSkill = &TargetItem->FPrimarySkill;

				/* Check the item target type and if it's usable on the target */
				switch (TargetSkill->ETargetType)
				{
					//---------------------------------------------------------------------------
				case EUsageTarget::UT_NONE: /* item doesn't require a target */
					bIsUsable = true;
					break;
					//---------------------------------------------------------------------------
				case EUsageTarget::UT_PLAYER: /* item requires another player as target */
											  //todo: create a seperate function

											  /* get the max target distance to use the item */
					TargetDistance = TargetSkill->fTargetDistanceMax;

					/* cast a traceline and check if hit actor is a valid target */
					RV_Hit = TraceLine((this->GetFirstPersonCameraComponent()->GetForwardVector() * 100) + this->GetFirstPersonCameraComponent()->GetComponentLocation(), (this->GetFirstPersonCameraComponent()->GetForwardVector() * TargetDistance) + this->GetFirstPersonCameraComponent()->GetComponentLocation(), true);

					if (RV_Hit.GetActor() != NULL)
					{
						AThingsInWoodsCharacter* TargetCharacter = Cast<AThingsInWoodsCharacter>(RV_Hit.GetActor());

						/* Target is a valid player character */
						if (TargetCharacter != nullptr)
						{
							/* Check if player is a valid target for the item (e.g. is low health on healthpack usage) */
							if (TargetItem->checkPrimary()) //todo: Add player target actor reference
							{
								/* player character is a valid target, set as action target and enable usage */
								this->AActionTarget = TargetCharacter;
								bIsUsable = true;
							}
						}

					}

					break;
					//---------------------------------------------------------------------------
				case EUsageTarget::UT_WORLD: /* item requires a location in world as target */
											 //todo: add WORLD Target function
					break;
					//---------------------------------------------------------------------------
				default:
					break;
				}

				/* use item if all requirements are fullfilled */
				if (bIsUsable)
				{
					/* check if the action is instant or requires time */
					if (TargetSkill->bIsOverTime)
					{
						/* over time use (e.g. healing) - execute instant */
						TargetItem->Primary();
					}
					else
					{
						/* instant use (e.g. rifle shot) - enable use over time */
						TargetItem->Primary();
						this->bIsUsingItem = true;
						this->bIsUsingOverTime = true;
						this->bIsUsingPrimary = true;
						this->fUsingPercentage = 0.0f; // set use percentage to zero
					}
				}
			}
		}
	}
}

bool AThingsInWoodsCharacter::Primary_Validate()
{
	return true;
}

void AThingsInWoodsCharacter::Primary_Implementation()
{
	/* Client */
	if (!HasAuthority())
	{
		/* execute clientside first */
		this->execPrimary();

		/* replicate to server */
		this->Server_Primary();
	}
	else /* Server */
	{
		this->Server_Primary();
	}
}

bool AThingsInWoodsCharacter::Server_Primary_Validate()
{
	return true;
}

void AThingsInWoodsCharacter::Server_Primary_Implementation()
{
	/* execute serverside */
	this->execPrimary();

	/* replicate to all other clients except owning client / server */
	this->Multi_Primary();
}

bool AThingsInWoodsCharacter::Multi_Primary_Validate()
{
	return true;
}

void AThingsInWoodsCharacter::Multi_Primary_Implementation()
{
	/* Only execute on other clients, not on owning client / server */
	/* Controller does only exist locally (owning client and server), so return if it does */
	if (this->GetController() != nullptr)
	{
		return;
	}

	this->execPrimary();
}


void AThingsInWoodsCharacter::execStopPrimary()
{
	if (this->bIsUsingItem && this->bIsUsingOverTime && this->bIsUsingPrimary)
	{
		int iInventoryStackIndex = this->iInventorySelectedObject;
		TArray<ABaseItem*>* Stack = &this->Inventory[iInventoryStackIndex].Item;
		int iStackSize = Stack->Num();

		if (iStackSize > 0)
		{
			ABaseItem* UseObject = (*Stack)[0];

			if (UseObject->IsValidLowLevel())
			{
				/* stop usage on object */
				UseObject->StopPrimary();
			}

			/* stop usage on character */
			this->bIsUsingItem = false;
			this->bIsUsingOverTime = false;
			this->fUsingPercentage = 0.0f;
		}
	}
}

bool AThingsInWoodsCharacter::StopPrimary_Validate()
{
	return true;
}

void AThingsInWoodsCharacter::StopPrimary_Implementation()
{
	/* client */
	if (!HasAuthority())
	{
		/* execute clientside first*/
		this->execStopPrimary();

		/* replicate to server */
		this->Server_StopPrimary();
	}
	else /* server */
	{
		this->Server_StopPrimary();
	}
}

bool AThingsInWoodsCharacter::Server_StopPrimary_Validate()
{
	return true;
}

void AThingsInWoodsCharacter::Server_StopPrimary_Implementation()
{
	/* stop server side */
	this->execStopPrimary();

	/* replicate to others except owning client / server */
	this->Multi_StopPrimary();
}

bool AThingsInWoodsCharacter::Multi_StopPrimary_Validate()
{
	return true;
}

void AThingsInWoodsCharacter::Multi_StopPrimary_Implementation()
{
	/* Only execute on other clients, not on owning client / server */
	/* Controller does only exist locally (owning client and server), so return if it does */
	if (this->GetController() != nullptr)
	{
		return;
	}
	
	this->execStopPrimary();
}

void AThingsInWoodsCharacter::execSecondary()
{
	int iInventoryStackIndex = this->iInventorySelectedObject;
	if (iInventoryStackIndex != -1)
	{
		TArray<ABaseItem*>* InventoryStack = &this->Inventory[iInventoryStackIndex].Item;

		if (InventoryStack->Num() > 0)
		{
			ABaseItem* TargetItem = (*InventoryStack)[0];
			if (TargetItem->IsValidLowLevel())
			{
				bool bIsUsable = false;
				float TargetDistance;
				FHitResult RV_Hit(ForceInit);
				FUsage* TargetSkill = &TargetItem->FSecondarySkill;

				/* Check the item target type and if it's usable on the target */
				switch (TargetSkill->ETargetType)
				{
					//---------------------------------------------------------------------------
				case EUsageTarget::UT_NONE: /* item doesn't require a target */
					bIsUsable = true;
					break;
					//---------------------------------------------------------------------------
				case EUsageTarget::UT_PLAYER: /* item requires another player as target */
											  //todo: create a seperate function

											  /* get the max target distance to use the item */
					TargetDistance = TargetSkill->fTargetDistanceMax;

					/* cast a traceline and check if hit actor is a valid target */
					RV_Hit = TraceLine((this->GetFirstPersonCameraComponent()->GetForwardVector() * 100) + this->GetFirstPersonCameraComponent()->GetComponentLocation(), (this->GetFirstPersonCameraComponent()->GetForwardVector() * TargetDistance) + this->GetFirstPersonCameraComponent()->GetComponentLocation(), true);

					if (RV_Hit.GetActor() != NULL)
					{
						AThingsInWoodsCharacter* TargetCharacter = Cast<AThingsInWoodsCharacter>(RV_Hit.GetActor());

						/* Target is a valid player character */
						if (TargetCharacter != nullptr)
						{
							/* Check if player is a valid target for the item (e.g. is low health on healthpack usage) */
							if (TargetItem->checkSecondary()) //todo: Add player target actor reference
							{
								/* player character is a valid target, set as action target and enable usage */
								this->AActionTarget = TargetCharacter;
								bIsUsable = true;
							}
						}

					}

					break;
					//---------------------------------------------------------------------------
				case EUsageTarget::UT_WORLD: /* item requires a location in world as target */
											 //todo: add WORLD Target function
					break;
					//---------------------------------------------------------------------------
				default:
					break;
				}

				/* use item if all requirements are fullfilled */
				if (bIsUsable)
				{
					/* check if the action is instant or requires time */
					if (TargetSkill->bIsOverTime)
					{
						/* over time use (e.g. healing) - execute instant */
						TargetItem->Secondary();
					}
					else
					{
						/* instant use (e.g. rifle shot) - enable use over time */
						TargetItem->Secondary();
						this->bIsUsingItem = true;
						this->bIsUsingOverTime = true;
						this->bIsUsingPrimary = false;
						this->fUsingPercentage = 0.0f; // set use percentage to zero
					}
				}
			}
		}
	}
}

bool AThingsInWoodsCharacter::Secondary_Validate()
{
	return true;
}

void AThingsInWoodsCharacter::Secondary_Implementation()
{
	/* Client */
	if (!HasAuthority())
	{
		/* execute clientside first */
		this->execSecondary();

		/* replicate to server */
		this->Server_Secondary();
	}
	else /* Server */
	{
		this->Server_Secondary();
	}
}

bool AThingsInWoodsCharacter::Server_Secondary_Validate()
{
	return true;
}

void AThingsInWoodsCharacter::Server_Secondary_Implementation()
{
	/* execute serverside */
	this->execSecondary();

	/* replicate to all other clients except owning client / server */
	this->Multi_Secondary();
}

bool AThingsInWoodsCharacter::Multi_Secondary_Validate()
{
	return true;
}

void AThingsInWoodsCharacter::Multi_Secondary_Implementation()
{
	/* Only execute on other clients, not on owning client / server */
	/* Controller does only exist locally (owning client and server), so return if it does */
	if (this->GetController() != nullptr)
	{
		return;
	}

	this->execSecondary();
}

void AThingsInWoodsCharacter::execStopSecondary()
{
	if (this->bIsUsingItem && this->bIsUsingOverTime && !this->bIsUsingPrimary)
	{
		int iInventoryStackIndex = this->iInventorySelectedObject;
		TArray<ABaseItem*>* Stack = &this->Inventory[iInventoryStackIndex].Item;
		int iStackSize = Stack->Num();

		if (iStackSize > 0)
		{
			ABaseItem* UseObject = (*Stack)[0];

			if (UseObject->IsValidLowLevel())
			{
				/* stop usage on object */
				UseObject->StopSecondary();
			}

			/* stop usage on character */
			this->bIsUsingItem = false;
			this->bIsUsingOverTime = false;
			this->fUsingPercentage = 0.0f;
		}
	}
}

bool AThingsInWoodsCharacter::StopSecondary_Validate()
{
	return true;
}

void AThingsInWoodsCharacter::StopSecondary_Implementation()
{
	/* client */
	if (!HasAuthority())
	{
		/* execute clientside first*/
		this->execStopSecondary();

		/* replicate to server */
		this->Server_StopSecondary();
	}
	else /* server */
	{
		this->Server_StopSecondary();
	}
}

bool AThingsInWoodsCharacter::Server_StopSecondary_Validate()
{
	return true;
}

void AThingsInWoodsCharacter::Server_StopSecondary_Implementation()
{
	/* stop server side */
	this->execStopSecondary();

	/* replicate to others except owning client / server */
	this->Multi_StopSecondary();
}

bool AThingsInWoodsCharacter::Multi_StopSecondary_Validate()
{
	return true;
}

void AThingsInWoodsCharacter::Multi_StopSecondary_Implementation()
{
	/* Only execute on other clients, not on owning client / server */
	/* Controller does only exist locally (owning client and server), so return if it does */
	if (this->GetController() != nullptr)
	{
		return;
	}
	
	this->execStopSecondary();
}


void AThingsInWoodsCharacter::execItemUsageHandle()
{
	/* be sure we're using an item and it's over time */
	if (this->bIsUsingItem && this->bIsUsingOverTime)
	{
		int iUsedItemIndex = this->iInventorySelectedObject;
		TArray<ABaseItem*>* InventoryStack = &this->Inventory[iUsedItemIndex].Item;

		if (InventoryStack->Num() > 0)
		{
			/* get the first item and be sure it's valid */
			ABaseItem* TargetItem = (*InventoryStack)[0];
			if (TargetItem->IsValidLowLevel())
			{
				/* set the character using percentage */
				this->fUsingPercentage = TargetItem->currentUsageProgression;

				bool bIsUsable = false;
				EUsageTarget EUseTarget;
				float fRequiredDistance;

				/* Get the Target Type NONE / OTHER PLAYER / IN WORLD */
				if (this->bIsUsingPrimary)
				{
					EUseTarget = TargetItem->FPrimarySkill.ETargetType;
					fRequiredDistance = TargetItem->FPrimarySkill.fTargetDistanceMax;
				}
				else {
					EUseTarget = TargetItem->FSecondarySkill.ETargetType;
					fRequiredDistance = TargetItem->FSecondarySkill.fTargetDistanceMax;
				}

				//Todo: See Primary / Secondary, replace
				/* check if the required target stuff is still valid */
				switch (EUseTarget)
				{
				case EUsageTarget::UT_NONE: /* No Target, always usable */
					bIsUsable = true;
					break;
				case EUsageTarget::UT_PLAYER: /* only other player as target */
					if (this->AActionTarget->IsValidLowLevel())
					{
						/* currently only calculate the distance between player character and other character */
						//todo: probably raycast
						if (this->GetDistanceTo(this->AActionTarget) <= fRequiredDistance)
						{
							bIsUsable = true;
						}
					}
					break;
				case EUsageTarget::UT_WORLD: /* world as target */
					break; //todo
				default:
					break;
				}

				/* Target requirements are okay, use the item */
				if (bIsUsable)
				{
					/* is item ready to use - on 100% */
					if (this->fUsingPercentage >= 100.0f)
					{
						/* execute primary / secondary */
						if (this->bIsUsingPrimary)
						{
							TargetItem->execPrimary(); //todo: probably replace because replication
							this->StopPrimary();
						}
						else
						{
							TargetItem->execSecondary(); //todo: probably replace because replication
							this->StopSecondary();
						}
					}

				}
				else /* target requirements are not met, cancel the action */
				{
					//Lets Cancel
					if (this->bIsUsingPrimary)
					{
						this->StopPrimary();  //todo: probably replace because replication
					}
					else
					{
						this->StopSecondary();  //todo: probably replace because replication
					}
				}
			}
		}
	}
}

bool AThingsInWoodsCharacter::ItemUsageHandle_Validate()
{
	return true;
}

void AThingsInWoodsCharacter::ItemUsageHandle_Implementation()
{
	/* Only execute serverside */
	/* Client */
	if (!HasAuthority())
	{
		//this->Server_ItemUsageHandle();
	}
	else /* Server */
	{
		/* run on server and replicate to all others */
		this->Server_ItemUsageHandle();
	}
}

bool AThingsInWoodsCharacter::Server_ItemUsageHandle_Validate()
{
	return true;
}

void AThingsInWoodsCharacter::Server_ItemUsageHandle_Implementation()
{
	this->Multi_ItemUsageHandle();
}

bool AThingsInWoodsCharacter::Multi_ItemUsageHandle_Validate()
{
	return true;
}

void AThingsInWoodsCharacter::Multi_ItemUsageHandle_Implementation()
{
	/* replicate to all clients and run on server */
	this->execItemUsageHandle();
}

void AThingsInWoodsCharacter::UseUsageObject(ABaseItem* object)
{
	object->UseObject();
}


void AThingsInWoodsCharacter::PlantObject()
{
	//todo
/*	if (this->InventorySelectedObject != -1)
	{
		if (this->Inventory[this->InventorySelectedObject].Item.Num() > 0)
		{
			FHitResult RV_Hit(ForceInit);
			RV_Hit = TraceLine((this->GetActorForwardVector() * 100) + this->GetFirstPersonCameraComponent()->GetComponentLocation(), (this->GetFirstPersonCameraComponent()->GetForwardVector() * 1000) + this->GetFirstPersonCameraComponent()->GetComponentLocation(), true);
			FVector PlaceLocation = (this->GetActorForwardVector() * 100) + this->GetActorLocation();
			this->Inventory[this->InventorySelectedObject].Item[0]->DetachRootComponentFromParent();
			this->Inventory[this->InventorySelectedObject].Item[0]->SetState(EObjectState::OS_PLACED, PlaceLocation);
			this->Inventory[this->InventorySelectedObject].Item[0]->ActivateObject();
			//this->Inventory[this->InventorySelectedObject].Item[0]->SetActorRotation(this->GetActorRotation());
			//Lets Cancel
			if (this->isUsingPrimary)
			{
				this->StopPrimary();
			}
			else
			{
				this->StopSecondary();
			}
			
			this->Inventory[this->InventorySelectedObject].Item.RemoveAt(0);

			this->InventoryDeselect();
		}
	}*/
}

bool AThingsInWoodsCharacter::HasNightVisionInInventory()
{
	//todo
	/*
	for (int i = 0; i < this->Inventory.Num(); i++)
	{
		if (this->Inventory[i].Item.Num() > 0)
		{
			if (Cast<ANightvision>(this->Inventory[i].Item[0]) != nullptr)
			{
				return true;
			}
		}
	}
	*/
	return false;
}

void AThingsInWoodsCharacter::ToggleNightVision()
{
	//todo
	/*
	if (!this->NightVisionEnabled)
	{
		if (this->HasNightVisionInInventory())
		{
			this->NightVisionEnabled = !this->NightVisionEnabled;
		}
		else
		{
			//Has no nightvision
			return;
		}
	}
	else
	{
		this->NightVisionEnabled = !this->NightVisionEnabled;
	}

	const FString command = TEXT("ToggleNightVisionBP");
	FOutputDeviceDebug debug;
	this->CallFunctionByNameWithArguments(*command, debug, this, true);*/
}
//------------------------------------------------------------------------
//------------------------------------------------------------------------
//------------------------------------------------------------------------
//SLENDERMAN GAMEMODE
//------------------------------------------------------------------------
void AThingsInWoodsCharacter::ReceiveSlenderScare(float Amount)
{
	/*GEngine->AddOnScreenDebugMessage(2, 1.0f, FColor::Green, FString::FromInt(Amount));
	this->Scare_Current += Amount;
	if (this->Scare_Current > this->Scare_Max)
	{
		this->Scare_Current = this->Scare_Max;
	}*/
}

void AThingsInWoodsCharacter::HandleScare(float DeltaTime)
{
	/*this->Scare_Current -= this->ScareFactorReduce * DeltaTime;
	if (this->Scare_Current < 0.0f)
	{
		this->Scare_Current = 0.0f;
	}
	GEngine->AddOnScreenDebugMessage(1, 1.0f, FColor::Yellow, FString::FromInt(this->Scare_Current));

	if (GetGameTimeSinceCreation() > this->NextScarePostProcessUpdateTime)
	{
		const FString command = TEXT("HandleSlanderScareBP");
		FOutputDeviceDebug debug;
		this->CallFunctionByNameWithArguments(*command, debug, this, true);

		this->NextScarePostProcessUpdateTime = GetGameTimeSinceCreation() + 0.25f;
	}

	if (this->Scare_Current > 0.0f)
	{
		if (!this->Slender_Scare_Playing)
		{
			//Scare sound
			this->Slender_Scare_Playing = true;
			this->VarAudioComponent->SetSound(this->Slender_Scare);
			this->VarAudioComponent->Play();
		}

		float fScare_volume = this->Scare_Current / this->Scare_Max;
		fScare_volume *= 3.0f;
		if (fScare_volume > 1.0f)
		{
			fScare_volume = 1.0f;
		}

		this->VarAudioComponent->SetVolumeMultiplier(fScare_volume);
	}
	else
	{
		this->Slender_Sound_Stop();
	}

	//Check for Damage
	if (this->Scare_Current > (this->Scare_Max / 100 * 50))
	{
		this->ApplyDamage((this->Scare_Current / 1000) * 10 * DeltaTime);
	}*/
}

void AThingsInWoodsCharacter::Slender_Sound_Stop()
{
	/*
	if (this->Slender_Scare_Playing)
	{
		this->Slender_Scare_Playing = false;
		this->VarAudioComponent->Stop();
	}*/
}