// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/LightComponent.h"
#include "LightModifierComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FLightIntensityDelegate, float, lightIntensity);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FLightFlickerDelegate, bool, isFlickering);
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DEEPSEAHORROR_API ULightModifierComponent : public UActorComponent
{
private:
	GENERATED_BODY()

	bool m_IsOn{false};
	
	bool m_IsFlickering{false};
	
	float m_CurrentIntensity{0.0f};

protected:

	virtual void BeginPlay() override;

public:	
	ULightModifierComponent();

	void AddLightToControlGroup(ULightComponent* pLightComponent);

	void AddMeshToControlGroup(UMeshComponent* pMeshComponent);
	
UFUNCTION(BlueprintCallable)
	void SwitchOn(bool force = false);

UFUNCTION(BlueprintCallable)
	void SwitchOff(bool force = false);
	
UFUNCTION(BlueprintCallable)	
	void SetLightIntensity(float intensity = 1.0f);

public:	

	UPROPERTY(BlueprintAssignable)
	FLightIntensityDelegate m_LightIntensityDelegate;

	UPROPERTY(BlueprintAssignable)
	FLightFlickerDelegate m_LightFlickerDelegate;

	UPROPERTY(EditAnywhere)
	UMaterialInterface* Material;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flicker");
	float m_MinFlickerBrightness;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flicker");
	float m_MaxFlickerBrightness;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flicker");
	float m_LightFlickerFrequency;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flicker");
	float m_LightDesiredPercentOnline;
	
	UPROPERTY(EditAnywhere, Category = "Material Setup")
	FRuntimeFloatCurve m_LightIntensityToEmissivity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LightSetting");
	float m_DefaultBrightness;

	UPROPERTY(EditAnywhere, Category = "LightSetting");
	bool m_DefaultOn;

	UPROPERTY(EditAnywhere)
	UMaterialInstanceDynamic* m_InstancedMat;

	UPROPERTY()
	TArray<ULightComponent*> m_pLightComponentsArray;
	
	UPROPERTY()
	TArray<UMeshComponent*> m_pMeshComponentsArray;
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
