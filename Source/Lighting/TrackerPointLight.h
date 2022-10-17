// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LightSensitivityComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "Gameplay/LightModifierComponent.h"
#include "TrackerPointLight.generated.h"

UCLASS()
class DEEPSEAHORROR_API ATrackerPointLight : public AActor
{
	GENERATED_BODY()

	std::vector<ULightSensitivityComponent*> m_pLightSensitivityComponents;

	bool IsProvidingEnoughLight() const;

	UFUNCTION()
	void OnSetLightIntensity(float lightIntensity);

public:	
	// Sets default values for this actor's properties
	ATrackerPointLight();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

	UFUNCTION()
		void OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
		void OnBoxEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	UPROPERTY(EditAnywhere)
	UPointLightComponent* m_pPointLight;
	
	UPROPERTY(EditAnywhere)
	USphereComponent* m_pCollisionShape;

	UPROPERTY(EditAnywhere)
	ULightModifierComponent* m_pLightModifier;

	UPROPERTY(EditAnywhere)
	float m_MinIntensityToBeConsideredOn;

	bool m_bIsBrightEnough = false;
};
