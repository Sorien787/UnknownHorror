// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIAlertLevel.h"
#include "UObject/ObjectKey.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "Perception/AIPerceptionComponent.h"
#include "SchismAIController.generated.h"


// in already loud environments, more sound shouldnt be an issue
// in already visually stimulating environments more visuals shouldnt be an issue

/**
 * 
 */
UENUM(BlueprintType)
enum class ESchismDetectionState : uint8
{
	NONE = 0,
	CHASE = 1,
	SKULK = 2,
	SNEAK = 3,
	COUNT = 4
};

UCLASS()
class DEEPSEAHORROR_API ASchismAIController : public AAIController
{
	
	GENERATED_BODY()

	ASchismAIController();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void GetActorEyesViewPoint(FVector& OutLocation, FRotator& OutRotation) const override;

	void OnReceiveNewStimulus(AActor* Actor, FAIStimulus Stimulus);
	void StimulusLoadUpdate(float DeltaSeconds);
	void OnAlertDataUpdate(float DeltaSeconds);
	void OnBlackboardVarsUpdate();
	void OnVisibleActorUpdate();
	void TickAlertData(FActorAlertData& alertData, float DeltaTime);
	void OnBestActorAlertDataUpdate();
	
	void TryExchangeAlertData(const AActor* pBestActor, const ISharedActorAlertData* bestActorAlertData, const AActor* pNewActor, const FActorAlertData& newActorAlertData) const;
	void DebugDrawAlertData(const FActorAlertData& AlertData) const;
	bool ShouldRemovePerceptionData(const FActorAlertData& AlertData) const;


	
	UFUNCTION(BlueprintCallable)
	EAIAlertLevel GetCurrentAlertLevel();

	UFUNCTION(BlueprintCallable)
	FVector GetCurrentPointOfInterest();

	UFUNCTION(BlueprintCallable)
	AActor* GetCurrentActorOfInterest();

	// downgrades the alert level for a given actor to a maximum, sets the perception that maximum as well
	// obviously, if the actor makes an amount of stimulus thereafter, this interest can be re-upgraded
	UFUNCTION(BlueprintCallable)
	void DowngradeAlertLevelForActor(AActor* pActor, EAIAlertLevel maximumAlertLevel);

	// Gets the last known location for the most interesting detection
	UFUNCTION(BlueprintCallable)
	FVector GetLastDetectionLocation(EAIAlertLevel alertLevel);

	UFUNCTION(BlueprintCallable)
	void LoseInterestInActor(AActor* pActor);

	UFUNCTION(BlueprintCallable)
	void ClearLoseInterestInActor(AActor* pActor);
	
	TMap<TObjectKey<AActor>, FActorAlertData> m_AudioAlertData;
	TMap<TObjectKey<AActor>, FActorAlertData> m_VisualAlertData;
	TSet<TObjectKey<AActor>> m_LostInterestSet;
	TMap<EAIAlertLevel, FVector> m_LastActorDetectionLocations;

	FDefaultActorAlertData m_DefaultAlertData;
	ISharedActorAlertData* m_BestActorAlertData;
	AActor* m_BestActorOfInterest = nullptr;
	bool m_bActorVisible = false;

	UFUNCTION()
	void OnTargetPerceptionUpdate(AActor* Actor, FAIStimulus Stimulus);
	
public:
	 UPROPERTY(EditAnywhere)
	 UAIPerceptionComponent* m_pMyPerceptionComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perception")
	FName m_EyeSocketName;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behaviour")
	UBehaviorTree* m_pBTAsset;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behaviour")
	FName AlertLevelBlackboardKey;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behaviour")
	FName TargetLocationBlackboardKey;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behaviour")
	FName HasVisuallyDetectedPlayerBlackboardKey;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perception")
	FRuntimeFloatCurve m_AngleFromCenterToVisualFalloffModifier;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perception")
	FRuntimeFloatCurve m_ImportanceSensitivityScalar;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perception")
	FRuntimeFloatCurve m_CompetingDesensitivityScalar;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perception")
	float m_MinimumTimeBeforeCulling;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perception")
	TMap<EAIAlertLevel, FAlertLevelDetectionThreshold> m_AlertLevelThresholds;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Perception")
	class UAISenseConfig_Hearing* m_pHearingConfig = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Perception")
	class UAISenseConfig_Sight* m_pSightConfig = nullptr;
};
