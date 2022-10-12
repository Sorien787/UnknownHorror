// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/LightComponent.h"
#include "LightModifierComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FLightIntensityDelegate, float, lightIntensity);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FLightFlickerDelegate, bool, isFlickering);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FLightBreakDelegate);

USTRUCT(BlueprintType)
struct FLightFlickerStateStruct
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite)
	float m_MinFlickerBrightness;

	UPROPERTY(BlueprintReadWrite)
	float m_MaxFlickerBrightness;

	UPROPERTY(BlueprintReadWrite)
	float m_LightFlickerFrequency;

	UPROPERTY(BlueprintReadWrite)
	float m_LightDesiredPercentOnline;

	FLightFlickerStateStruct();

	FLightFlickerStateStruct(float min, float brightness, float freq, float proportion);
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DEEPSEAHORROR_API ULightModifierComponent : public UActorComponent
{
private:
	GENERATED_BODY()

	bool m_IsBroken{false};

	bool m_IsOn{false};
	
	bool m_IsFlickering{false};
	
	float m_CurrentIntensity{0.0f};

	float m_TimeLastStateTransition{0.0f};

	FLightFlickerStateStruct m_CurrentLightFlickerState;

protected:

	virtual void BeginPlay() override;

public:	
	ULightModifierComponent();

	void AddLightToControlGroup(ULightComponent* pLightComponent);

	void AddMeshToControlGroup(UMeshComponent* pMeshComponent);

	void Break();

	void SetFlickerStatusOverride(const FLightFlickerStateStruct& flickerStateOverride);

	void CancelFlickerStatusOverride();
	
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

	UPROPERTY(BlueprintAssignable)
	FLightBreakDelegate m_LightBreakDelegate;

	UPROPERTY(EditAnywhere, Category = "Material Setup");
	UMaterialInterface* Material;
	
	UPROPERTY(EditAnywhere, Category = "Material Setup")
	FRuntimeFloatCurve m_LightIntensityToEmissivity;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LightSetting");
	FLightFlickerStateStruct m_DefaultLightFlickerState;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LightSetting");
	float m_DefaultBrightness;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LightSetting");
	bool m_DefaultOn;

	UPROPERTY()
	UMaterialInstanceDynamic* m_InstancedMat;

	UPROPERTY()
	TArray<ULightComponent*> m_pLightComponentsArray;
	
	UPROPERTY()
	TArray<UMeshComponent*> m_pMeshComponentsArray;
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
