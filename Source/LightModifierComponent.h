// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HazeEffectComponent.h"
#include "Components/ActorComponent.h"
#include "Components/LightComponent.h"
#include "LightModifierComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DEEPSEAHORROR_API ULightModifierComponent : public UActorComponent, public IHazeModifierComponent
{
	GENERATED_BODY()

	bool m_IsOn{false};
	bool m_IsFlickering{false};
	float m_CurrentIntensity{0.0f};
public:	
	// Sets default values for this component's properties
	ULightModifierComponent();

	void AddLightToControlGroup(ULightComponent* pLightComponent);
UFUNCTION(BlueprintCallable)
	void SwitchOn(bool force = false);

UFUNCTION(BlueprintCallable)
	void SwitchOff(bool force = false);
	
UFUNCTION(BlueprintCallable)	
	void SetLightIntensity(float intensity = 1.0f);

	virtual void OnHazeStrengthChanged(float hazeStrength) override;	

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY()
		TArray<ULightComponent*> m_pLightComponentsArray;
public:	
	// Called every frame

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flicker");
	float m_MinFlickerBrightness;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flicker");
	float m_MaxFlickerBrightness;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flicker");
	float m_LightFlickerFrequency;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flicker");
	float m_LightDesiredPercentOnline;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flicker");
	float m_LightFlickerSmoothness;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LightSetting");
	float m_DefaultBrightness;

	UPROPERTY(EditAnywhere, Category = "LightSetting");
	bool m_DefaultOn;
	
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
