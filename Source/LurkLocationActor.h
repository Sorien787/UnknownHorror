// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/AIAlertLevel.h"
#include "GameFramework/Actor.h"
#include "LurkLocationActor.generated.h"

UENUM(BlueprintType)
enum class EAIType : uint8
{
	Schism,
	Madonna,
	Crewmate
};

UCLASS()
class DEEPSEAHORROR_API ALurkLocationActor : public AActor
{
	GENERATED_BODY()

	bool m_IsActive = false;

public:	
	// Sets default values for this actor's properties
	ALurkLocationActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void ActivateLocation();

	UFUNCTION()
	void DeactivateLocation();

	bool GetIsActive() const;

	bool IsValidLurkType(EAIType AIType) const;
	
	UPROPERTY(EditAnywhere)
	bool m_DefaultActive = true;

	UPROPERTY(EditAnywhere)
	UAnimMontage* m_LurkAnim;

	UPROPERTY(EditAnywhere)
	TArray<EAIType> m_ValidType;
};
