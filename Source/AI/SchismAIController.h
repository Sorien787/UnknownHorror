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

UINTERFACE(MinimalAPI, NotBlueprintable)
class UAIControllerShared : public UInterface
{
	GENERATED_BODY()
};

class DEEPSEAHORROR_API IAIControllerShared
{
	GENERATED_BODY()
public:
	
	UFUNCTION(BlueprintCallable)
	virtual void DowngradeAlertLevelForActor(AActor* pActor, EAIAlertLevel maximumAlertLevel) = 0;

	UFUNCTION(BlueprintCallable)
	virtual void LoseInterestInActor(AActor* pActor) = 0;

	UFUNCTION(BlueprintCallable)
	virtual void ClearLoseInterestInActor(AActor* pActor) = 0;
	
	UFUNCTION(BlueprintCallable)
	virtual bool GetIsInterestedObjectVisible() = 0;

	UFUNCTION(BlueprintCallable)
	virtual void OnSearchObject(const AActor* pActor) = 0;

	UFUNCTION(BlueprintCallable)
	virtual AActor*  OnReceiveObjectSearchList(const TArray<AActor*>& inActors, bool& result) = 0;
};

UCLASS()
class DEEPSEAHORROR_API ASchismAIController : public AAIController, public IAIControllerShared
{
	
	GENERATED_BODY()

	ASchismAIController();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void GetActorEyesViewPoint(FVector& OutLocation, FRotator& OutRotation) const override;

	void OnReceiveNewStimulus(AActor* Actor, FAIStimulus Stimulus);
	void StimulusLoadUpdate(float DeltaSeconds);
	void OnAlertDataUpdate(float DeltaSeconds);
	void OnBlackboardVarsUpdate(float DeltaSeconds);
	void OnVisibleActorUpdate();
	void TickAlertData(FActorAlertData& alertData, float DeltaTime);
	void OnBestActorAlertDataUpdate();
	
	void TryExchangeAlertData(AActor* &pBestActor, ISharedActorAlertData* &bestActorAlertData, AActor* pNewActor, FActorAlertData& newActorAlertData) const;
	void DebugDrawAlertData(const FActorAlertData& AlertData) const;
	bool ShouldRemovePerceptionData(const FActorAlertData& AlertData) const;


	
	UFUNCTION(BlueprintCallable)
	EAIAlertLevel GetCurrentAlertLevel();

	UFUNCTION(BlueprintCallable)
	FVector GetCurrentPointOfInterest();

	UFUNCTION(BlueprintCallable)
	AActor* GetCurrentActorOfInterest();

	UFUNCTION(BlueprintCallable)
	FVector GetLastDetectionLocation(EAIAlertLevel alertLevel);
	

	//////////////////////////////////////////////////////////////////
	//IAIControllerShared
	UFUNCTION(BlueprintCallable)
	virtual void DowngradeAlertLevelForActor(AActor* pActor, EAIAlertLevel maximumAlertLevel) override;
	
	UFUNCTION(BlueprintCallable)
	virtual void LoseInterestInActor(AActor* pActor) override;
	
	UFUNCTION(BlueprintCallable)
	virtual void ClearLoseInterestInActor(AActor* pActor) override;
	
	UFUNCTION(BlueprintCallable)
	virtual bool GetIsInterestedObjectVisible() override;
	
	UFUNCTION(BlueprintCallable)
	virtual void OnSearchObject(const AActor* pActor) override;

	UFUNCTION(BlueprintCallable)
	virtual AActor* OnReceiveObjectSearchList(const TArray<AActor*>& inActors, bool& result) override;
	
	//~IAIControllerShared
	//////////////////////////////////////////////////////////////////
	///
	UFUNCTION()
	void OnTargetPerceptionUpdate(AActor* Actor, FAIStimulus Stimulus);
	
	TMap<TObjectKey<AActor>, FActorAlertData> m_AudioAlertData;
	TMap<TObjectKey<AActor>, FActorAlertData> m_VisualAlertData;
	TSet<TObjectKey<AActor>> m_LostInterestSet;
	TMap<EAIAlertLevel, FVector> m_LastActorDetectionLocations;
	TMap<TObjectKey<AActor>, float> m_HidingSpotSearchList;
	FDefaultActorAlertData m_DefaultAlertData;
	ISharedActorAlertData* m_BestActorAlertData;
	AActor* m_BestActorOfInterest = nullptr;
	bool m_bActorVisible = false;
	float m_ObservedByPlayerTime = 0.0f;
	float m_NotObservedByPlayerTime = 0.0f;
	
public:
	
	 UPROPERTY(EditAnywhere)
	 UAIPerceptionComponent* m_pMyPerceptionComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perception")
	float m_ResearchTimeoutTime;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perception")
	FName m_EyeSocketName;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behaviour")
	UBehaviorTree* m_pBTAsset;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behaviour")
	FName AlertLevelBlackboardKey;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behaviour")
	FName MostInterestingLocationBlackboardKey;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behaviour")
	FName MostInterestingActorBlackboardKey;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behaviour")
	FName HasVisuallyDetectedPlayerBlackboardKey;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behaviour")
	FName RelevantSearchRadiusBlackboardKey;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behaviour")
	FName SearchProbabilityByPerceptionBlackboardKey;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behaviour")
	FName ObservedByPlayerScaleBlackboardKey;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behaviour")
	FName NotObservedByPlayerScaleBlackboardKey;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perception")
	FRuntimeFloatCurve m_AngleFromCenterToVisualFalloffModifier;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perception")
	FRuntimeFloatCurve m_ImportanceSensitivityScalar;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perception")
	FRuntimeFloatCurve m_CompetingDesensitivityScalar;
		
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perception")
	FRuntimeFloatCurve m_SearchPrecisionByCurrentAlertness;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perception")
	FRuntimeFloatCurve m_SearchProbabilityByCurrentAlertness;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perception")
	float m_MinimumTimeBeforeCulling;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perception")
	TMap<EAIAlertLevel, FAlertLevelDetectionThreshold> m_AlertLevelThresholds;

};
