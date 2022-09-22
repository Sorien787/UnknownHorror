// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractionTriggerInterface.h"
#include "GameFramework/Actor.h"
#include "ProximityInteraction.generated.h"

UCLASS()
class DEEPSEAHORROR_API AProximityInteraction : public AActor, public IInteractionTriggerInterface
{
private:
	GENERATED_BODY()

public:	
	// Sets default values for this actor's properties
	AProximityInteraction();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

	virtual void Tick(float DeltaTime) override;

	virtual FTransform GetInteractorTransform() const override {return GetActorTransform();}

	virtual int GetInteractorId() const override {return m_interactorId;}
	
	virtual bool GetActorTriggerConditionMet(FTransform actorTransform, FVector actorVelocity) const override;
	
	UPROPERTY()
	USceneComponent* m_RootComponent;

	UPROPERTY(EditAnywhere, Category = "Interaction Settings")
	int m_interactorId{0};

	UPROPERTY(EditAnywhere, Category = "Interaction Settings",  meta = (ClampMin = "0.0", ClampMax = "180.0", UIMin = "0.0", UIMax = "180.0"))
	float m_TriggerAngle{90};

	UPROPERTY(EditAnywhere, Category = "Interaction Settings")
	bool m_RequireMovementInDirection{true};

};
