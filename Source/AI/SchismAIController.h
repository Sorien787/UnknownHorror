// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIAlertLevel.h"
#include "UObject/ObjectKey.h"
#include "AIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "SchismAIController.generated.h"

/**
 * 
 */
UCLASS()
class DEEPSEAHORROR_API ASchismAIController : public AAIController
{
	
	GENERATED_BODY()

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	virtual void GetActorEyesViewPoint(FVector& OutLocation, FRotator& OutRotation) const override;
	
	ASchismAIController();

	UFUNCTION(BlueprintCallable)
	EAIAlertLevel GetCurrentAlertLevel();

	UFUNCTION(BlueprintCallable)
	FVector GetCurrentPointOfInterest();

	UFUNCTION(BlueprintCallable)
	AActor* GetCurrentActorOfInterest();

	
	TMap<TObjectKey<AActor>, FActorAlertData> m_CachedAlertData;

	void OnBestActorAlertDataUpdate();

	void OnTargetPositionUpdate();

	TObjectKey<AActor> m_BestActorAlertData;
	
public:
	 UPROPERTY(EditAnywhere)
	 UAIPerceptionComponent* m_pMyPerceptionComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perception")
	FName m_EyeSocketName;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behaviour")
	UBehaviorTree* m_pBTAsset;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perception")
	FRuntimeFloatCurve m_AngleFromCenterToVisualFalloffModifier;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perception")
	float m_MinimumPerceptionBeforeCulling;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perception")
	float m_MaximumDefaultPerception;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perception")
	float m_MinimumTimeBeforeCulling;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perception")
	TMap<EAIAlertLevel, FAlertLevelDetectionThreshold> m_AlertLevelThresholds;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Perception")
	class UAISenseConfig_Hearing* m_pHearingConfig = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Perception")
	class UAISenseConfig_Sight* m_pSightConfig = nullptr;
};
