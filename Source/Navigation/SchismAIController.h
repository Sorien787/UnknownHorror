// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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

	virtual void GetActorEyesViewPoint(FVector& OutLocation, FRotator& OutRotation) const override;
	
	ASchismAIController();
	
	UFUNCTION()
	void OnActorPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);
	
	UFUNCTION()
	void OnActorPerceptionInfoUpdated(const FActorPerceptionUpdateInfo& UpdateInfo);
	
	UFUNCTION()
	void PerceptionUpdated(const TArray<AActor*>& UpdatedActors);
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAIPerceptionComponent* m_pPerceptionComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName m_EyeSocketName;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBehaviorTree* m_pBTAsset;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Perception")
	class UAISenseConfig_Hearing* m_pHearingConfig = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Perception")
	class UAISenseConfig_Sight* m_pSightConfig = nullptr;
};
