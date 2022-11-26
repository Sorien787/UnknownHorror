// Fill out your copyright notice in the Description page of Project Settings.


#include "Navigation/SchismAIController.h"
#pragma optimize("", off)
#include "Common/UnrealUtilities.h"
#include "Delegates/DelegateSignatureImpl.inl"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISenseConfig_Sight.h"

void ASchismAIController::BeginPlay()
{
	RunBehaviorTree(m_pBTAsset);
	m_pPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this,  &ASchismAIController::OnActorPerceptionUpdated);
	m_pPerceptionComponent->OnPerceptionUpdated.AddDynamic(this,  &ASchismAIController::PerceptionUpdated);
	m_pPerceptionComponent->OnTargetPerceptionInfoUpdated.AddDynamic(this,  &ASchismAIController::OnActorPerceptionInfoUpdated);
}

void ASchismAIController::GetActorEyesViewPoint(FVector& OutLocation, FRotator& OutRotation) const
{
	const USkeletalMeshComponent* pSkeletalMesh = UnrealUtilities::GetComponentFromActor<USkeletalMeshComponent>(GetPawn());
	if (!pSkeletalMesh)
	{
		Super::GetActorEyesViewPoint(OutLocation, OutRotation);
		return;
	}

	const int32 boneIndex = pSkeletalMesh->GetBoneIndex(m_EyeSocketName);
	if (INDEX_NONE == boneIndex)
	{
		Super::GetActorEyesViewPoint(OutLocation, OutRotation);
		return;
	}
	OutLocation = pSkeletalMesh->GetBoneTransform(boneIndex).GetLocation();
	OutRotation = pSkeletalMesh->GetBoneTransform(boneIndex).GetRotation().Rotator();
}

ASchismAIController::ASchismAIController()
{
	m_pPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>("Perception Component");
	check(m_pPerceptionComponent != nullptr);
	SetPerceptionComponent(*m_pPerceptionComponent);
	m_pPerceptionComponent->bEditableWhenInherited = true;

	

	m_pSightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Sense"));
	m_pHearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("Hearing Sense"));

	m_pSightConfig->DetectionByAffiliation.bDetectEnemies = true;
	m_pSightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	m_pSightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	
	m_pPerceptionComponent->ConfigureSense(*m_pSightConfig);

	m_pHearingConfig->DetectionByAffiliation.bDetectEnemies = true;
	m_pHearingConfig->DetectionByAffiliation.bDetectFriendlies = true;
	m_pHearingConfig->DetectionByAffiliation.bDetectNeutrals = true;
	m_pPerceptionComponent->ConfigureSense(*m_pHearingConfig);
}

void ASchismAIController::OnActorPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	int i = 0;
	i++;
}

void ASchismAIController::OnActorPerceptionInfoUpdated(const FActorPerceptionUpdateInfo& UpdateInfo)
{
	int i = 0;
	i++;
}

void ASchismAIController::PerceptionUpdated(const TArray<AActor*>& UpdatedActors)
{
	int i = 0;
	i++;
}

