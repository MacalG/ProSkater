// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/SKTBasePlayer.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/Engine.h"
#include "InputActionValue.h"
#include <Widgets/Player/SKTPlayerHUD.h>

// Sets default values
ASKTBasePlayer::ASKTBasePlayer()
{
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	CollectableCollision = CreateDefaultSubobject<USphereComponent>(TEXT("CollectableCollision"));
	CollectableCollision->SetupAttachment(RootComponent);
	CollectableCollision->InitSphereRadius(50.f);
	CollectableCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollectableCollision->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel1);
	CollectableCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CollectableCollision->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);

	GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -90.f));
	GetMesh()->SetRelativeRotation(FQuat(0.f, 0.f, 90.f, 0.f));

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 500.f;
	CameraBoom->SetRelativeLocation(FVector(0.f, 0.f, 70.f));
	CameraBoom->TargetOffset = FVector(0.f, 0.f, 180.f);
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;
}

// Called when the game starts or when spawned
void ASKTBasePlayer::BeginPlay()
{
	Super::BeginPlay();
	
	if (PlayerPointsWidgetClass)
    {
        PlayerPointsWidgetInstance = Cast<USKTPlayerHUD>(CreateWidget(GetWorld(), PlayerPointsWidgetClass));
        if (PlayerPointsWidgetInstance)
        {
            PlayerPointsWidgetInstance->AddToViewport();
            PlayerPointsWidgetInstance->UpdatePoints(Points);
        }
    }
}

void ASKTBasePlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CurrentSpeed > 0.f)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		AddMovementInput(ForwardDirection, CurrentSpeed * DeltaTime);
	}
}

// Called to bind functionality to input
void ASKTBasePlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {

		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ASKTBasePlayer::StartJump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Look
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ASKTBasePlayer::Look);

		// Speed Up
		EnhancedInputComponent->BindAction(SpeedUpAction, ETriggerEvent::Triggered, this, &ASKTBasePlayer::SpeedUp);

		//Slow Down
		EnhancedInputComponent->BindAction(SlowDownAction, ETriggerEvent::Triggered, this, &ASKTBasePlayer::SlowDown);
	}
}

void ASKTBasePlayer::AddPoints(int32 Amount)
{
	if (Amount <= 0) return;

	Points += Amount;
	if (PlayerPointsWidgetInstance)
	{
		PlayerPointsWidgetInstance->UpdatePoints(Points);
	}
}

void ASKTBasePlayer::Look(const FInputActionValue& Value)
{
	if (!FollowCamera) return;
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		AddControllerYawInput(LookAxisVector.X * 0.5f);
	}
}

void ASKTBasePlayer::SpeedUp()
{
	if (CurrentSpeed <= 0.f)
	{
		CurrentSpeed = 300.f;
	}
	else
	{
		CurrentSpeed += AccelerationRate;
		if (CurrentSpeed > MaxSpeed)
		{
			CurrentSpeed = MaxSpeed;
		}
	}

	GetCharacterMovement()->MaxWalkSpeed = CurrentSpeed;

	if (SpeedUpMontage && GetMesh()->GetAnimInstance())
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

		if (!AnimInstance->Montage_IsPlaying(SpeedUpMontage))
		{
			AnimInstance->Montage_Play(SpeedUpMontage);
		}

		float PlayRate = FMath::GetMappedRangeValueClamped(
			FVector2D(0.f, MaxSpeed),
			FVector2D(0.5f, 2.0f),
			CurrentSpeed
		);
		AnimInstance->Montage_SetPlayRate(SpeedUpMontage, PlayRate);
	}
}

void ASKTBasePlayer::SlowDown()
{
	CurrentSpeed -= DecelerationRate;
	if (CurrentSpeed < MinSpeed)
	{
		CurrentSpeed = MinSpeed;
	}
	GetCharacterMovement()->MaxWalkSpeed = CurrentSpeed;
}

void ASKTBasePlayer::StartJump()
{
	if (JumpMontage && GetMesh()->GetAnimInstance())
	{
		GetMesh()->GetAnimInstance()->Montage_Play(JumpMontage);
	}

	Jump();
}

