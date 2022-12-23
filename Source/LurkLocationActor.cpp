// Fill out your copyright notice in the Description page of Project Settings.


#include "LurkLocationActor.h"

#include "Gameplay/TensionSubsystem.h"

// Sets default values
ALurkLocationActor::ALurkLocationActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ALurkLocationActor::BeginPlay()
{
	Super::BeginPlay();
	UWorld* pWorld = GetWorld();
	if (!pWorld)
		return;

	UTensionSubsystem* pTension = pWorld->GetSubsystem<UTensionSubsystem>();
	if (!pTension)
		return;

	pTension->RegisterLurkLocation(this);
}

// Called every frame
void ALurkLocationActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ALurkLocationActor::ActivateLocation()
{
	m_IsActive = true;
}

void ALurkLocationActor::DeactivateLocation()
{
	m_IsActive = false;
}

bool ALurkLocationActor::GetIsActive() const
{
	return m_IsActive;
}

UAnimMontage* ALurkLocationActor::GetMontage() const
{
	return m_MontageToPlay;
}

bool ALurkLocationActor::IsValidLurkType(EAIType AIType) const
{
	for (const auto& it : m_ValidType)
	{
	    if(AIType == it)
	    	return true;
	}
	return false;
}

