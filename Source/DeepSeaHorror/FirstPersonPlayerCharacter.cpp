// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "FirstPersonPlayerCharacter.h"

// Sets default values
AFirstPersonPlayerCharacter::AFirstPersonPlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create a first person camera component.
	m_CharacterCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	check(m_CharacterCamera != nullptr);

	// Attach the camera component to our capsule component.
	m_CharacterCamera->SetupAttachment(CastChecked<USceneComponent, UCapsuleComponent>(GetCapsuleComponent()));
	// Position the camera slightly above the eyes.
	m_CharacterCamera->SetRelativeLocation(FVector(0.0f, 0.0f, 50.0f + BaseEyeHeight));
	// Enable the pawn to control camera rotation.
	m_CharacterCamera->bUsePawnControlRotation = true;
	
	m_InteractionComponent = CreateDefaultSubobject<UInteractionUserComponent>(TEXT("InteractionUser"));
	check(m_InteractionComponent != nullptr);
	
	m_EnterBoxComponent= CreateDefaultSubobject<UBoxComponent>(TEXT("Interaction Enter Box"));
	check(m_EnterBoxComponent != nullptr);
	m_EnterBoxComponent->SetupAttachment(GetRootComponent());

	m_ExitBoxComponent= CreateDefaultSubobject<UBoxComponent>(TEXT("Interaction Exit Box"));
	check(m_ExitBoxComponent != nullptr);
	m_ExitBoxComponent->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void AFirstPersonPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	m_InteractionComponent->AddInteractionEnterBox(m_EnterBoxComponent);
	m_InteractionComponent->AddInteractionExitBox(m_ExitBoxComponent);
}

// Called every frame
void AFirstPersonPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AFirstPersonPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AFirstPersonPlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AFirstPersonPlayerCharacter::MoveRight);

	PlayerInputComponent->BindAxis("LookRight", this, &AFirstPersonPlayerCharacter::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &AFirstPersonPlayerCharacter::AddControllerPitchInput);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AFirstPersonPlayerCharacter::StartJump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &AFirstPersonPlayerCharacter::StopJump);

	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &AFirstPersonPlayerCharacter::StartInteractions);
}


void AFirstPersonPlayerCharacter::MoveForward(float Value)
{
	FVector Direction = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::X);
	AddMovementInput(Direction, Value);
}

void AFirstPersonPlayerCharacter::MoveRight(float Value)
{
	FVector Direction = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::Y);
	AddMovementInput(Direction, Value);
}

void AFirstPersonPlayerCharacter::StartJump()
{
	bPressedJump = true;
}

void AFirstPersonPlayerCharacter::StopJump()
{
	bPressedJump = false;
}

void AFirstPersonPlayerCharacter::StartInteractions()
{
	m_InteractionComponent->OnInteractButtonPressed();
}



