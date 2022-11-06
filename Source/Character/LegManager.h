// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "LegComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "LegManager.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DEEPSEAHORROR_API ULegManager : public USceneComponent
{
	GENERATED_BODY()

	FFloatSpringState m_fCurrentBodyTranslationState;
	
	float m_fCurrentOffsetHeight = 0.0f;

	float m_fMaxPawnSpeed = 0.0f;
public:	

	ULegManager();

protected:

	virtual void BeginPlay() override;

public:

	void UpdateDesiredBodyHeightDefaultRelative(float DeltaTime);

	void UpdateLegs(float DeltaTime);
		
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	FVector GetCurrentRaisedBodyPosition() const;
	
private:
	UPROPERTY(EditAnywhere, Category = "Animation | Controlled Legs")
	TArray<ULegComponent*> m_controllableLegComponents;

	UPROPERTY(EditAnywhere, Category = "Animation | Controlled Legs")
	TArray<FLegRaycastProfile> m_LegRaycastProfiles;

	UPROPERTY(EditAnywhere, Category = "Animation | Step Parameters")
	float LegUnplantThreshold{1.0f};
	
	UPROPERTY(EditAnywhere, Category = "Animation | Step Parameters")
	FRuntimeFloatCurve m_StepDistanceByMovementSpeed;
	
	UPROPERTY(EditAnywhere, Category = "Animation | Step Parameters")
	FRuntimeFloatCurve m_StepTimeByMovementSpeed;

	UPROPERTY(EditAnywhere, Category = "Animation | Step Parameters")
	FRuntimeFloatCurve m_ExtraStepDistanceByMovementSpeed;
	
	UPROPERTY(EditAnywhere, Category = "Animation | Step Parameters")
	FRuntimeFloatCurve m_StepHeightByMovementSpeed;

	UPROPERTY(EditAnywhere, Category = "Animation | Body Translation")
	UBoxComponent* m_pBoxComponentForGroundCollision;
	
	UPROPERTY(EditAnywhere, Category = "Animation | Body Translation")
	float m_BoxTraceHeightUp;

	UPROPERTY(EditAnywhere, Category = "Animation | Body Translation")
	float m_BoxTraceHeightDown;

	UPROPERTY(EditAnywhere, Category = "Animation | Body Translation")
	float m_BodyTranslationSpeed;

	UPROPERTY(EditAnywhere, Category = "Animation | Body Translation")
	float m_BodyTranslationDamping;
};
