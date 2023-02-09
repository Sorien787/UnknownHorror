// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LightModifierFlickerStruct.h"
#include "Components/ActorComponent.h"
#include "ItemControl/ItemControlComponent.h"
#include "Components/LightComponent.h"
#include "ItemControl/ItemControlComponent.h"
#include "LightModifierComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FLightIntensityDelegate, float, lightIntensity);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FLightFlickerDelegate, bool, isFlickering);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FLightBreakDelegate);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DEEPSEAHORROR_API ULightModifierComponent : public UItemControlComponent, public IItemControlRequester
{
private:
	GENERATED_BODY()
	
	float m_TimeLastStateTransition{0.0f};

	FTimerHandle m_BreakTimerHandle;

	FLightFlickerStateStruct m_CurrentLightFlickerState;
	
	UPROPERTY()
	UMaterialInstanceDynamic* m_InstanceBulbMat;

	UPROPERTY()
	TArray<ULightComponent*> m_pLightComponentsArray;
	
	UPROPERTY()
	TArray<UMeshComponent*> m_pMeshComponentsArray;
protected:

	virtual void BeginPlay() override;

public:	
	ULightModifierComponent();

	void AddLightToControlGroup(ULightComponent* pLightComponent);

	void AddMeshToControlGroup(UMeshComponent* pMeshComponent);

	void OnFinishedBreaking();

	virtual void OnItemControlGranted_Implementation(AActor* pControlledActor) override;

	virtual void OnItemControlLost_Implementation(AActor* pControlledActor) override {};

	virtual int GetPriority() const override {return 0;}
	
	void Break();

	void SetFlickerStatusOverride(const FLightFlickerStateStruct& flickerStateOverride);

	void CancelFlickerStatusOverride();
	
UFUNCTION(BlueprintCallable)
	void PowerOn(bool force = false);

UFUNCTION(BlueprintCallable)
	void PowerOff(bool force = false);
	
UFUNCTION(BlueprintCallable)	
	void SetIntensityScalar(float intensity = 1.0f);

public:	

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool m_IsBroken{false};

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool m_IsOn{false};

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool m_IsFlickering{false};

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float m_CurrentIntensity{0.0f};

	UPROPERTY(BlueprintAssignable)
	FLightIntensityDelegate m_LightIntensityDelegate;

	UPROPERTY(BlueprintAssignable)
	FLightFlickerDelegate m_LightFlickerDelegate;

	UPROPERTY(BlueprintAssignable)
	FLightBreakDelegate m_LightBreakDelegate;

	UPROPERTY(EditAnywhere, Category = "Material Setup");
	UMaterialInterface* m_BulbMaterial;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material Setup")
	FRuntimeFloatCurve m_LightIntensityToEmissivity;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LightSetting");
	FLightFlickerStateStruct m_DefaultLightFlickerState;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LightSetting");
	float m_DefaultBrightness;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LightSetting");
	bool m_DefaultOn;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LightSetting");
	float m_BrightnessWhenBreaking {3.0f};
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LightSetting");
	float m_LengthOfBreakFlash{0.3f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LightSetting");
	bool m_ApplyFrequencyScalar;
	

	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
