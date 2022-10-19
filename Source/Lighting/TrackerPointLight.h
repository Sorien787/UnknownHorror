// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LightSensitivityComponent.h"
#include "Components/PointLightComponent.h"
#include "TrackerPointLight.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DEEPSEAHORROR_API UTrackerPointLight : public UActorComponent
{
	GENERATED_BODY()

	std::vector<ULightSensitivityComponent*> m_pLightSensitivityComponents;

	bool IsProvidingEnoughLight() const;

	void OnActorOverlap(AActor* pActor);
	
	UFUNCTION()
	void OnSetLightIntensity(float lightIntensity);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void InitializeComponent() override;
public:	

	UFUNCTION()
		void OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
		void OnBoxEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	UPointLightComponent* m_pPointLight;

	UPROPERTY(EditAnywhere)
	float m_MinIntensityToBeConsideredOn;

	bool m_bIsBrightEnough = false;
};
