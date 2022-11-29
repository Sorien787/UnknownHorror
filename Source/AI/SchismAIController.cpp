// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SchismAIController.h"
#include "PerceptionTypeComponent.h"
#include "Common/UnrealUtilities.h"
#include "Delegates/DelegateSignatureImpl.inl"
#include "Engine/SkeletalMeshSocket.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISenseConfig_Sight.h"

static TAutoConsoleVariable<int32> AIPerceptionDebug(
	TEXT("AIPerceptionDebug"),
	0,
	TEXT("Shows AI Perception debug information.\n"),
	ECVF_Scalability | ECVF_RenderThreadSafe);


//////////////////////////////////////////////////////////////////////////////////////////
void ASchismAIController::BeginPlay()
{
	Super::BeginPlay();
	RunBehaviorTree(m_pBTAsset);
	PrimaryActorTick.bCanEverTick = true;
	SetActorTickEnabled(true);
}

//////////////////////////////////////////////////////////////////////////////////////////
void ASchismAIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	TMap<TObjectKey<AActor>, FActorPerceptionInfo>::TIterator iterator = m_pMyPerceptionComponent->GetPerceptualDataIterator();
	for (; iterator; ++iterator)
	{
		AActor* pActor = iterator->Key.ResolveObjectPtr();
		
		if (!pActor)
		{
			m_CachedAlertData.Remove(iterator->Key);
			continue;
		}
		const UPerceptionTypeComponent* pPerceptionTypeComponent = UnrealUtilities::GetComponentFromActor<UPerceptionTypeComponent>(pActor);

		FVector actorLocation = iterator.Key().ResolveObjectPtr()->GetActorLocation();
		
		FActorPerceptionInfo perceptionInfo = iterator->Value;
		
		for (size_t i = 0; i < perceptionInfo.LastSensedStimuli.Num(); i++)
		{
			const FAIStimulus& lastStimulus = perceptionInfo.LastSensedStimuli[i];
			if (lastStimulus.GetAge() > FLT_EPSILON || !lastStimulus.IsActive())
				continue;
	
			if (!m_CachedAlertData.Contains(pActor))
			{
				EAIAlertLevel defaultMaximumAlertLevel = EAIAlertLevel::INVESTIGATING;
				uint8 defaultPriority = 0;
				float maximumInterest = m_MaximumDefaultPerception;
	
				if (pPerceptionTypeComponent)
				{
					defaultMaximumAlertLevel = pPerceptionTypeComponent->GetMaximumAlertLevel();
					defaultPriority = pPerceptionTypeComponent->GetPriority();
					maximumInterest = pPerceptionTypeComponent->GetMaximumInterest();
				}
				m_CachedAlertData.Add(pActor, FActorAlertData( defaultMaximumAlertLevel, defaultPriority, maximumInterest));
			}

			FActorAlertData& alertData = m_CachedAlertData[pActor];
			const float normalizedPerceptionStrength = alertData.CurrentPerceptionStrength/alertData.MaximumInterest;
			const float distanceFromSource = GetPawn() ? (pActor->GetActorLocation() - GetPawn()->GetActorLocation()).Length() : 0.0f;
			float strengthMultiplier = 1.0f;
			if (lastStimulus.Type == UAISense::GetSenseID<UAISense_Sight>())
			{
				if (pPerceptionTypeComponent)
				{
					const float perceptionGeneralScalar = pPerceptionTypeComponent->GetVisualPerceptionModifier();
					const float perceptionDistanceScalar = pPerceptionTypeComponent->GetVisualPerceptionDistanceScalar(distanceFromSource);
					FVector vec;
					FRotator rot;
					GetActorEyesViewPoint(vec, rot);
					const float perceptionAngleScalar =  m_AngleFromCenterToVisualFalloffModifier.EditorCurveData.Eval(FMath::RadiansToDegrees(UnrealUtilities::GetRadAngleBetweenVectors(rot.Quaternion().GetForwardVector(), pActor->GetActorLocation()-vec))); 
					strengthMultiplier = DeltaSeconds * perceptionDistanceScalar * perceptionGeneralScalar * perceptionAngleScalar;
					
				}
				else
					strengthMultiplier = DeltaSeconds * strengthMultiplier;
			}
			else if (lastStimulus.Type == UAISense::GetSenseID<UAISense_Hearing>())
			{
				if (pPerceptionTypeComponent)
				{
					strengthMultiplier *= pPerceptionTypeComponent->GetAudioPerceptionDistanceScalar(distanceFromSource);
				}
			}
			alertData.CurrentPerceptionStrength += strengthMultiplier * lastStimulus.Strength;
			alertData.LastTimeStimulusReceived = GetWorld()->GetTimeSeconds();
		}
		if (!m_CachedAlertData.Contains(pActor))
			continue;
		
		FActorAlertData& AlertData = m_CachedAlertData[pActor];

		float defaultPerceptionDecay = 1.0f;
		if (pPerceptionTypeComponent)
			defaultPerceptionDecay = pPerceptionTypeComponent->GetPerceptionDecay(AlertData.CurrentPerceptionStrength / AlertData.MaximumInterest);
		AlertData.CurrentPerceptionStrength -= DeltaSeconds * defaultPerceptionDecay;
		AlertData.CurrentPerceptionStrength = FMath::Clamp(AlertData.CurrentPerceptionStrength, 0.0f, AlertData.MaximumInterest);
		// our perception strength has decreased - check if we need to drop down an alert level
		EAIAlertLevel& currentAlertLevel = AlertData.CurrentAlertLevel;
	
		if (currentAlertLevel > EAIAlertLevel::IDLE)
		{
			const EAIAlertLevel lowerAlertLevel = (EAIAlertLevel) ((uint8)currentAlertLevel-1);
			const FAlertLevelDetectionThreshold& threshold = m_AlertLevelThresholds[lowerAlertLevel];

			if (AlertData.CurrentPerceptionStrength < threshold.LowDetectionThreshold)
				AlertData.CurrentAlertLevel = lowerAlertLevel;
		}
	
		if (currentAlertLevel < EAIAlertLevel::DETECTION)
		{
			const EAIAlertLevel maximumAlertLevel = AlertData.MaxAlertLevel;
			const EAIAlertLevel higherAlertLevel = (EAIAlertLevel)(FMath::Min((uint8)((uint8)currentAlertLevel + 1u), (uint8)maximumAlertLevel));
			const FAlertLevelDetectionThreshold& threshold = m_AlertLevelThresholds[higherAlertLevel];
	
			if (AlertData.CurrentPerceptionStrength > threshold.HighDetectionThreshold)
				AlertData.CurrentAlertLevel = higherAlertLevel;
		}
		
		if ((AlertData.CurrentPerceptionStrength < m_MinimumPerceptionBeforeCulling) && (GetWorld()->GetTimeSeconds() - AlertData.LastTimeStimulusReceived) > m_MinimumTimeBeforeCulling)
		{
			m_CachedAlertData.Remove(pActor);
			return;
		}

		if (!AIPerceptionDebug.GetValueOnGameThread())
			continue;


		// remove the alert data since it's not really relevant anymore.

		DrawDebugString(GetWorld(), actorLocation, *FString::Printf(TEXT("Current perception strength: %f \n Maximum perception: %f\n Maximum alert level: %s\n Current alert level: %s\n Priority: %i\n"),
			AlertData.CurrentPerceptionStrength,
			AlertData.MaximumInterest,
			*StaticEnum<EAIAlertLevel>()->GetValueAsString(AlertData.MaxAlertLevel),
			*StaticEnum<EAIAlertLevel>()->GetValueAsString(AlertData.CurrentAlertLevel),
			AlertData.Priority), nullptr, FColor::White, 0.05f);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void ASchismAIController::GetActorEyesViewPoint(FVector& OutLocation, FRotator& OutRotation) const
{
	const APawn* pawn = GetPawn();

	if (!pawn)
		return Super::GetActorEyesViewPoint(OutLocation, OutRotation);
	
	const USkeletalMeshComponent* pSkeletalMesh = UnrealUtilities::GetComponentFromActor<USkeletalMeshComponent>(GetPawn());
	
	if (!pSkeletalMesh)
		return Super::GetActorEyesViewPoint(OutLocation, OutRotation);

	const USkeletalMeshSocket* socket = pSkeletalMesh->GetSocketByName(m_EyeSocketName);
	
	if (!socket)
		return Super::GetActorEyesViewPoint(OutLocation, OutRotation);
	
	OutLocation = socket->GetSocketTransform(pSkeletalMesh).GetLocation();
	OutRotation = socket->GetSocketTransform(pSkeletalMesh).GetRotation().Rotator();
}

//////////////////////////////////////////////////////////////////////////////////////////
ASchismAIController::ASchismAIController()
{
	m_pMyPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>("AI Perception Component");
	check(m_pMyPerceptionComponent != nullptr);
	SetPerceptionComponent(*m_pMyPerceptionComponent);
	m_pMyPerceptionComponent->bEditableWhenInherited = true;
	
	PrimaryActorTick.bCanEverTick = true;
	
	m_pSightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Sense"));
	m_pHearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("Hearing Sense"));
	
	m_pSightConfig->DetectionByAffiliation.bDetectEnemies = true;
	m_pSightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	m_pSightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	
	m_pMyPerceptionComponent->ConfigureSense(*m_pSightConfig);
	
	m_pHearingConfig->DetectionByAffiliation.bDetectEnemies = true;
	m_pHearingConfig->DetectionByAffiliation.bDetectFriendlies = true;
	m_pHearingConfig->DetectionByAffiliation.bDetectNeutrals = true;
	m_pMyPerceptionComponent->ConfigureSense(*m_pHearingConfig);
}

EAIAlertLevel ASchismAIController::GetCurrentAlertLevel()
{
	if (m_BestActorAlertData.ResolveObjectPtr() == nullptr)
		return EAIAlertLevel::IDLE;
	return m_CachedAlertData[m_BestActorAlertData].CurrentAlertLevel;
}

FVector ASchismAIController::GetCurrentPointOfInterest()
{
	if (m_BestActorAlertData.ResolveObjectPtr() == nullptr)
		return FVector::ZeroVector;
	return m_BestActorAlertData.ResolveObjectPtr()->GetActorLocation();
}

AActor* ASchismAIController::GetCurrentActorOfInterest()
{
	return m_BestActorAlertData.ResolveObjectPtr();
}

void ASchismAIController::OnBestActorAlertDataUpdate()
{
	TMap<TObjectKey<AActor>, FActorAlertData>::TIterator iterator = TMap<TObjectKey<AActor>, FActorAlertData>::TIterator(m_CachedAlertData);
	FActorAlertData defaultActorAlertData = FActorAlertData(EAIAlertLevel::IDLE, 0, 0);
	FActorAlertData &currentBestAlertData = defaultActorAlertData;
	AActor* bestActor = nullptr;
	for (; iterator; ++iterator)
	{
		AActor* pActor = iterator.Key().ResolveObjectPtr();
		if (!pActor)
			continue;
		
		FActorAlertData& newAlertData = iterator.Value();
		
		// add priority to alert data to determine the object we should take most note of
		const uint8 totalPriority = (uint8)newAlertData.CurrentAlertLevel + newAlertData.Priority;
		const uint8 lastBestTotalPriority = (uint8)currentBestAlertData.CurrentAlertLevel + currentBestAlertData.Priority;
		// if last one had higher combined priority, take it.
		if (lastBestTotalPriority > totalPriority)
			continue;
		// if the combined priorities are equal, then ignore this new one if the priority is less (prioritise > priority)
		if (lastBestTotalPriority == totalPriority && newAlertData.Priority < currentBestAlertData.Priority)
			continue;

		currentBestAlertData = newAlertData;
		bestActor = pActor;
	}
	
	if (m_BestActorAlertData == TObjectKey<AActor>(bestActor))
		return;
	
	m_BestActorAlertData = TObjectKey<AActor>(bestActor);

}

void ASchismAIController::OnTargetPositionUpdate()
{
	// we have an exact point, but we want to be randomly roaming in the points' radius.
	//

	AActor* pActor = m_BestActorAlertData.ResolveObjectPtr();
	if (!pActor)
		return;

	FActorAlertData& alertData = m_CachedAlertData[m_BestActorAlertData];
	
	const UPerceptionTypeComponent* pPerceptionTypeComponent = UnrealUtilities::GetComponentFromActor<UPerceptionTypeComponent>(pActor);
	
	float uncertainty = pPerceptionTypeComponent->GetUncertaintyDistance(alertData.CurrentPerceptionStrength / alertData.MaximumInterest);
		
}

