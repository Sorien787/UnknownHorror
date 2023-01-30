// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SchismAIController.h"
#include "PerceptionTypeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Common/UnrealUtilities.h"
#include "Delegates/DelegateSignatureImpl.inl"
#include "Engine/SkeletalMeshSocket.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISightTargetInterface.h"

static TAutoConsoleVariable<int32> AIPerceptionDebug(
	TEXT("AIPerceptionDebug"),
	0,
	TEXT("Shows AI Perception debug information.\n"),
	ECVF_Scalability | ECVF_RenderThreadSafe);

//////////////////////////////////////////////////////////////////////////////////////////
void ASchismAIController::BeginPlay()
{
	Super::BeginPlay();
	m_pMyPerceptionComponent = UnrealUtilities::GetComponentFromActor<UAIPerceptionComponent>(this);
	check(m_pMyPerceptionComponent != nullptr);
	SetPerceptionComponent(*m_pMyPerceptionComponent);

	RunBehaviorTree(m_pBTAsset);
	PrimaryActorTick.bCanEverTick = true;
	SetActorTickEnabled(true);
	m_pMyPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &ASchismAIController::OnTargetPerceptionUpdate);
}

//////////////////////////////////////////////////////////////////////////////////////////
ASchismAIController::ASchismAIController()
{
	PrimaryActorTick.bCanEverTick = true;
}

//////////////////////////////////////////////////////////////////////////////////////////
void ASchismAIController::OnReceiveNewStimulus(AActor* Actor, FAIStimulus Stimulus)
{
	if ( !Actor || m_LostInterestSet.Contains(Actor))
		return;
	
	UPerceptionTypeComponent* pPerceptionTypeComponent = UnrealUtilities::GetComponentFromActor<UPerceptionTypeComponent>(Actor);

	if (!pPerceptionTypeComponent)
		return;
	
	const float DeltaSecondsWorld = GetWorld()->GetDeltaSeconds();

	FVector PerceptionSourceTranslation = FVector::ZeroVector;
	FRotator PerceptionSourceRotation = FRotator::ZeroRotator;
	GetActorEyesViewPoint(PerceptionSourceTranslation, PerceptionSourceRotation);
	const FVector OffsetFromSource = (Actor->GetActorLocation() - PerceptionSourceTranslation);
	float stimulusStrength = DeltaSecondsWorld * Stimulus.Strength;
	float totalOtherStimulusStrength = 0.0f;
	
	float perceptionRelevanceInterestScalar = 1;
	if(m_AudioAlertData.Contains(Actor))
	{
		perceptionRelevanceInterestScalar *= m_ImportanceSensitivityScalar.EditorCurveData.Eval(m_AudioAlertData[Actor].CurrentPerceptionStrength);
	}
	if (m_VisualAlertData.Contains(Actor))
	{
		perceptionRelevanceInterestScalar *= m_ImportanceSensitivityScalar.EditorCurveData.Eval(m_VisualAlertData[Actor].CurrentPerceptionStrength);
	}
	
	if (GEngine && AIPerceptionDebug.GetValueOnGameThread())
		GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Yellow, FString::Printf(TEXT("PerceptionAudioInterestScalar: %f"), perceptionRelevanceInterestScalar));
	const FPerceptionTypeModifier* mod = pPerceptionTypeComponent->GetPerceptionTypeModifierForSense(Stimulus.Type);
	if (Stimulus.Type == UAISense::GetSenseID<UAISense_Sight>())
	{
		const float perceptionGeneralScalar = pPerceptionTypeComponent->GetPerceptionTypeModifierForSense(Stimulus.Type)->m_PerceptionModifier;
		const float perceptionDistanceScalar = pPerceptionTypeComponent->GetPerceptionTypeModifierForSense(Stimulus.Type)->m_NewPerceptionFalloffModifierByDistance.EditorCurveData.Eval(OffsetFromSource.Length());

		const float fieldOfViewDegrees = static_cast<UAISenseConfig_Sight*>(m_pMyPerceptionComponent->GetSenseConfig(Stimulus.Type))->PeripheralVisionAngleDegrees;
		const float angularOffsetDegrees = FMath::RadiansToDegrees(UnrealUtilities::GetRadAngleBetweenVectors(PerceptionSourceRotation.Quaternion().GetForwardVector(), OffsetFromSource));
		const float perceptionAngleScalar =  m_AngleFromCenterToVisualFalloffModifier.EditorCurveData.Eval(angularOffsetDegrees / fieldOfViewDegrees); 
		stimulusStrength *= perceptionDistanceScalar * perceptionGeneralScalar * perceptionAngleScalar * perceptionRelevanceInterestScalar;
		stimulusStrength = FMath::Min(stimulusStrength, mod->m_MaxInterest);
		
		for (const TPair<TObjectKey<AActor>, FActorAlertData>& pair : m_VisualAlertData)
		{
			if (pair.Key.ResolveObjectPtr() && pair.Key.ResolveObjectPtr() == Actor)
				continue;
			totalOtherStimulusStrength += pair.Value.CurrentPerceptionStrength;
		}
		stimulusStrength *= m_CompetingDesensitivityScalar.EditorCurveData.Eval(totalOtherStimulusStrength);
		if (GEngine && AIPerceptionDebug.GetValueOnGameThread())
			GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Yellow, FString::Printf(TEXT("New Perception Strength Sight Event: %f"), stimulusStrength));
		
		if(!m_VisualAlertData.Contains(Actor))
		{
			m_VisualAlertData.Add(Actor,FActorAlertData(pPerceptionTypeComponent, Stimulus.Type, stimulusStrength, GetWorld()->GetTimeSeconds(),Stimulus.StimulusLocation));
		}
		else
		{
			m_VisualAlertData[Actor].EventTimeoutValue = 0.0f;
			m_VisualAlertData[Actor].EventTime = GetWorld()->GetTimeSeconds();
			m_VisualAlertData[Actor].CurrentPerceptionStrength += stimulusStrength;
			m_VisualAlertData[Actor].CurrentPerceptionStrength = FMath::Min(m_VisualAlertData[Actor].CurrentPerceptionStrength, mod->m_MaxInterest);
			m_VisualAlertData[Actor].EventLocation = Stimulus.StimulusLocation;
		}
		
	}
	else if (Stimulus.Type == UAISense::GetSenseID<UAISense_Hearing>())
	{
		const float perceptionGeneralScalar = pPerceptionTypeComponent->GetPerceptionTypeModifierForSense(Stimulus.Type)->m_PerceptionModifier;
		const float perceptionDistanceScalar = pPerceptionTypeComponent->GetPerceptionTypeModifierForSense(Stimulus.Type)->m_NewPerceptionFalloffModifierByDistance.EditorCurveData.Eval (OffsetFromSource.Length());
		stimulusStrength *= perceptionGeneralScalar * perceptionDistanceScalar * perceptionRelevanceInterestScalar;
		stimulusStrength = FMath::Min(stimulusStrength, mod->m_MaxInterest);
		for (const TPair<TObjectKey<AActor>, FActorAlertData>& pair : m_AudioAlertData)
		{
			if (pair.Key.ResolveObjectPtr() && pair.Key.ResolveObjectPtr() == Actor)
				continue;
			totalOtherStimulusStrength += pair.Value.CurrentPerceptionStrength;
		}
		stimulusStrength *= m_CompetingDesensitivityScalar.EditorCurveData.Eval(totalOtherStimulusStrength);
		if (!m_AudioAlertData.Contains(Actor))
		{
			m_AudioAlertData.Add(Actor, FActorAlertData(pPerceptionTypeComponent, Stimulus.Type,stimulusStrength, GetWorld()->GetTimeSeconds(), Stimulus.StimulusLocation));

			if (GEngine && AIPerceptionDebug.GetValueOnGameThread())
				GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, FString::Printf(TEXT("New Perception Strength Hearing Event: %f"), stimulusStrength));
		}
		else
		{
			FActorAlertData& currentAlertData = m_AudioAlertData[Actor];
			stimulusStrength = FMath::Min(perceptionRelevanceInterestScalar * stimulusStrength, mod->m_MaxInterest);
			
			if (GEngine && AIPerceptionDebug.GetValueOnGameThread())
				GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, FString::Printf(TEXT("New Perception Strength Hearing Event: %f"), stimulusStrength));
			if (GEngine && AIPerceptionDebug.GetValueOnGameThread())
				GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, FString::Printf(TEXT("Old Perception Strength Hearing Event: %f"), currentAlertData.CurrentPerceptionStrength));
	
			if (stimulusStrength < currentAlertData.CurrentPerceptionStrength)
				return;
			currentAlertData.EventTime = GetWorld()->GetTimeSeconds();
			currentAlertData.EventTimeoutValue = 0.0f;
			currentAlertData.CurrentPerceptionStrength = stimulusStrength;
			currentAlertData.EventLocation = Stimulus.StimulusLocation;
		}
	}

}

//////////////////////////////////////////////////////////////////////////////////////////
void ASchismAIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	StimulusLoadUpdate(DeltaSeconds);
	OnAlertDataUpdate(DeltaSeconds);
	OnBestActorAlertDataUpdate();
	OnVisibleActorUpdate();
	OnBlackboardVarsUpdate(DeltaSeconds);
}

//////////////////////////////////////////////////////////////////////////////////////////
void ASchismAIController::DebugDrawAlertData(const FActorAlertData& AlertData) const
{
	if (!AIPerceptionDebug.GetValueOnGameThread())
		return;
	// remove the alert data since it's not really relevant anymore.
	
	DrawDebugString(GetWorld(), AlertData.EventLocation, *FString::Printf(TEXT("Current perception strength: %f \n Current alert level: %s\n Priority: %i\n"),
		AlertData.CurrentPerceptionStrength,
		*StaticEnum<EAIAlertLevel>()->GetValueAsString(AlertData.CurrentAlertLevel),
		AlertData.pPerceptionComponent->GetPriority()), nullptr, FColor::White, 0.01f);
}

//////////////////////////////////////////////////////////////////////////////////////////
bool ASchismAIController::ShouldRemovePerceptionData(const FActorAlertData& AlertData) const
{
	return (AlertData.EventTimeoutValue > m_MinimumTimeBeforeCulling);
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
void ASchismAIController::StimulusLoadUpdate(float DeltaSeconds)
{
	auto it = m_pMyPerceptionComponent->GetPerceptualDataIterator();
	while(it)
	{
		for (size_t i = 0; i < it.Value().LastSensedStimuli.Num(); i++)
		{
			if(it.Value().LastSensedStimuli[i].Type != UAISense::GetSenseID<UAISense_Sight>())
				continue;
			if (it.Value().LastSensedStimuli[i].GetAge() > FLT_EPSILON)
				continue;
			
			OnReceiveNewStimulus(it.Key().ResolveObjectPtr(), it.Value().LastSensedStimuli[i]);
		}
		++it;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void ASchismAIController::OnAlertDataUpdate(float DeltaSeconds)
{
	for (TMap<TObjectKey<AActor>, FActorAlertData>::TIterator IterAlertData = m_AudioAlertData.CreateIterator(); IterAlertData; ++IterAlertData)
	{
		TickAlertData(IterAlertData->Value, DeltaSeconds);
		DebugDrawAlertData(IterAlertData->Value);
		
		if (!ShouldRemovePerceptionData(IterAlertData.Value()))
			continue;
		
		IterAlertData.RemoveCurrent();
	}
	
	for (TMap<TObjectKey<AActor>, FActorAlertData>::TIterator IterAlertData = m_VisualAlertData.CreateIterator(); IterAlertData; ++IterAlertData)
	{
		TickAlertData(IterAlertData.Value(), DeltaSeconds);
		DebugDrawAlertData(IterAlertData.Value());
		
		if (!ShouldRemovePerceptionData(IterAlertData.Value()))
			continue;
		
		IterAlertData.RemoveCurrent();
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void ASchismAIController::OnBlackboardVarsUpdate(float DeltaSeconds)
{
	Blackboard->SetValueAsVector(MostInterestingLocationBlackboardKey, m_BestActorAlertData->EventLocation);
	Blackboard->SetValueAsObject(MostInterestingActorBlackboardKey, m_BestActorOfInterest);
	Blackboard->SetValueAsEnum(AlertLevelBlackboardKey, static_cast<uint8>(m_BestActorAlertData->CurrentAlertLevel));
	Blackboard->SetValueAsBool(HasVisuallyDetectedPlayerBlackboardKey, m_bActorVisible);
	const float searchRadius = m_SearchPrecisionByCurrentAlertness.EditorCurveData.Eval(m_BestActorAlertData->CurrentPerceptionStrength);
	Blackboard->SetValueAsFloat( RelevantSearchRadiusBlackboardKey, searchRadius);
	const float searchProbability = m_SearchProbabilityByCurrentAlertness.EditorCurveData.Eval(m_BestActorAlertData->CurrentPerceptionStrength);
	Blackboard->SetValueAsFloat( SearchProbabilityByPerceptionBlackboardKey, searchProbability);

	FVector eyeLoc;
	FRotator eyeRot;
	FVector returnLoc;
	float outStr;
	int numLineChecks;
	GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorEyesViewPoint(eyeLoc, eyeRot);
	AActor* pawnActor =  GetWorld()->GetFirstPlayerController()->GetPawn();
	bool isVisible = UnrealUtilities::IsInFrustrum(GetPawn()->GetActorLocation(), GetPawn()->GetSimpleCollisionRadius(), GetWorld());
	APawn* schismPawn = GetPawn();
	IAISightTargetInterface* interf = Cast<IAISightTargetInterface>(schismPawn);
	isVisible =  isVisible && (!interf || interf->CanBeSeenFrom(eyeLoc, returnLoc, numLineChecks, outStr, pawnActor, nullptr, nullptr));
	if ( !isVisible )
	{
		m_ObservedByPlayerTime = 0.0f;
		m_NotObservedByPlayerTime += DeltaSeconds;
	}
	else
	{
		m_ObservedByPlayerTime += DeltaSeconds;
		m_NotObservedByPlayerTime = 0.0f;
	}
	Blackboard->SetValueAsFloat(ObservedByPlayerScaleBlackboardKey, m_ObservedByPlayerTime);
	Blackboard->SetValueAsFloat(NotObservedByPlayerScaleBlackboardKey, m_NotObservedByPlayerTime);
    	
	if (!AIPerceptionDebug.GetValueOnGameThread())
		return;

	if (m_BestActorOfInterest)
	{
		DrawDebugSphere(GetWorld(), m_BestActorAlertData->EventLocation, searchRadius, 60, FColor::White);

		DrawDebugSphere(GetWorld(), m_BestActorAlertData->EventLocation, 50.0f, 10, FColor::Red);
	}

	FString valA = m_bActorVisible ? "Yes" : "No";
	FString valB = m_BestActorOfInterest ? m_BestActorOfInterest->GetName() : "Invalid";
	FString str = FString::Printf(
		TEXT("MostInterestingActorBlackboardKey: %s, \n Current Alert Level: %s \n Has Visually Detected Most Interesting Actor: %s \n Relevant Search Radius: %f \n Search Probability: %f,\n Current perception value: %f, \n Time has been observed: %f, \n Time has not been observed: %f"),
		*valB,
		*UEnum::GetValueAsString(m_BestActorAlertData->CurrentAlertLevel),
		*valA,
		searchRadius,
		searchProbability,
		m_BestActorAlertData->CurrentPerceptionStrength,
		m_ObservedByPlayerTime,
		m_NotObservedByPlayerTime
		);
	GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Yellow,str );
}

//////////////////////////////////////////////////////////////////////////////////////////
void ASchismAIController::OnVisibleActorUpdate()
{
	m_bActorVisible = false;
	
	if(!m_BestActorOfInterest)
		return;

	FActorPerceptionBlueprintInfo Info;
	if (!m_pMyPerceptionComponent->GetActorsPerception(m_BestActorOfInterest, Info))
		return;
	
	const FAISenseID sightSenseID = UAISense::GetSenseID<UAISense_Sight>();
	const FAIStimulus& sightStimulus = Info.LastSensedStimuli[sightSenseID];
	// only actually in sight if age is less than float epsilon (just seen)
	m_bActorVisible = sightStimulus.GetAge() < FLT_EPSILON && m_BestActorAlertData->CurrentAlertLevel == EAIAlertLevel::DETECTION;
}

//////////////////////////////////////////////////////////////////////////////////////////
void ASchismAIController::TickAlertData(FActorAlertData& AlertData, float DeltaTime)
{
	const FPerceptionTypeModifier* mod = AlertData.pPerceptionComponent->GetPerceptionTypeModifierForSense(AlertData.SenseID);
	float decayDelay = mod->m_TimeBeforePerceptionDecayBegin;
	float age = GetWorld()->GetTimeSeconds() -  AlertData.EventTime;
	if (decayDelay < age)
	{
		const float currentPerceptionDecayScale = mod->m_PerceptionDecayMultiplierByCurrentPerception.EditorCurveData.Eval(AlertData.CurrentPerceptionStrength / mod->m_MaxInterest);
		const float ageDecayScale = mod->m_PerceptionDecayMultiplierByLifetime.EditorCurveData.Eval(age);
		const float totalModifier = AlertData.pPerceptionComponent->GetPerceptionTypeModifierForSense(AlertData.SenseID)->m_PerceptionDecayModifier;
		AlertData.CurrentPerceptionStrength -= DeltaTime * totalModifier * ageDecayScale * currentPerceptionDecayScale;
	}

	AlertData.CurrentPerceptionStrength = FMath::Max(0.0f, AlertData.CurrentPerceptionStrength);
	
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
		const EAIAlertLevel maximumAlertLevel =mod->m_MaxAlertLevel;
		const EAIAlertLevel higherAlertLevel = (EAIAlertLevel)(FMath::Min((uint8)((uint8)currentAlertLevel + 1u), (uint8)maximumAlertLevel));
		const FAlertLevelDetectionThreshold& threshold = m_AlertLevelThresholds[higherAlertLevel];
	
		if (AlertData.CurrentPerceptionStrength > threshold.HighDetectionThreshold)
			AlertData.CurrentAlertLevel = higherAlertLevel;
	}
	
	if (currentAlertLevel == EAIAlertLevel::IDLE)
	{
		AlertData.EventTimeoutValue += DeltaTime;
	}
	else
	{
		AlertData.EventTimeoutValue = 0.0f;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void ASchismAIController::TryExchangeAlertData(AActor* &pBestActor, ISharedActorAlertData* &bestActorAlertData, AActor* pNewActor, FActorAlertData& newActorAlertData) const
{
	// pick via alert level
	if (bestActorAlertData->CurrentAlertLevel > newActorAlertData.CurrentAlertLevel)
		return;
	// and then by perception strength
	if (bestActorAlertData->CurrentAlertLevel == newActorAlertData.CurrentAlertLevel && bestActorAlertData->CurrentPerceptionStrength > newActorAlertData.CurrentPerceptionStrength)
		return;

	// and then by ignored set
	if (m_LostInterestSet.Contains(pNewActor))
		return;
	
	bestActorAlertData = &newActorAlertData;
	pBestActor = pNewActor;
}

EAIAlertLevel ASchismAIController::GetCurrentAlertLevel()
{
	return m_BestActorAlertData->CurrentAlertLevel;
}

FVector ASchismAIController::GetCurrentPointOfInterest()
{
	return m_BestActorAlertData->EventLocation;
}

AActor* ASchismAIController::GetCurrentActorOfInterest()
{
	return m_BestActorOfInterest;
}

void ASchismAIController::DowngradeAlertLevelForActor(AActor* pActor, EAIAlertLevel maximumAlertLevel)
{
	const float detectionMaximum = (m_AlertLevelThresholds[maximumAlertLevel].HighDetectionThreshold + m_AlertLevelThresholds[maximumAlertLevel].LowDetectionThreshold) / 2;

	if (m_VisualAlertData.Contains(pActor))
	{
		FActorAlertData& alertData = m_VisualAlertData[pActor];
		
		if (alertData.CurrentAlertLevel < maximumAlertLevel)
			return;
		
		alertData.CurrentPerceptionStrength = FMath::Min(alertData.CurrentPerceptionStrength, detectionMaximum);
		alertData.CurrentAlertLevel = FMath::Min(alertData.CurrentAlertLevel, maximumAlertLevel);
	}
	if (m_AudioAlertData.Contains(pActor))
	{
		FActorAlertData& alertData = m_AudioAlertData[pActor];
		
		if(alertData.CurrentAlertLevel < maximumAlertLevel)
			return;

		alertData.CurrentPerceptionStrength = FMath::Min(alertData.CurrentPerceptionStrength, detectionMaximum);
		alertData.CurrentAlertLevel = FMath::Min(alertData.CurrentAlertLevel, maximumAlertLevel);
	}
}

FVector ASchismAIController::GetLastDetectionLocation(EAIAlertLevel alertLevel)
{
	if (!m_LastActorDetectionLocations.Contains(alertLevel))
		return FVector::ZeroVector;
	return m_LastActorDetectionLocations[alertLevel];
}

bool ASchismAIController::GetIsInterestedObjectVisible()
{
	ISharedActorAlertData* alertData = m_BestActorAlertData;
	return alertData->EventTimeoutValue < 0.1f;
}

void ASchismAIController::OnSearchObject(const AActor* pActor)
{
	m_HidingSpotSearchList.Add(pActor, GetWorld()->GetTimeSeconds());
}

AActor* ASchismAIController::OnReceiveObjectSearchList(const TArray<AActor*>& inActors, bool& result)
{
	for (AActor* actor : inActors)
	{
		if (!m_HidingSpotSearchList.Contains(actor))
		{
			result = true;
			return actor;
		}
		const float* time = m_HidingSpotSearchList.Find(actor);
		if (GetWorld()->GetTimeSeconds() - *time < m_ResearchTimeoutTime)
			continue;
		result = true;
		return actor;
	}
	result = false;
	return nullptr;
}


void ASchismAIController::LoseInterestInActor(AActor* pActor)
{
	UPerceptionTypeComponent* pPerceptionTypeComp = UnrealUtilities::GetComponentFromActor<UPerceptionTypeComponent>(pActor);
	if (!pPerceptionTypeComp->GetCanBeIgnored())
		return;
	m_LostInterestSet.Add(pActor);
}

void ASchismAIController::ClearLoseInterestInActor(AActor* pActor)
{
	m_LostInterestSet.Remove(pActor);
}

void ASchismAIController::OnTargetPerceptionUpdate(AActor* Actor, FAIStimulus Stimulus)
{
	if(Stimulus.Type != UAISense::GetSenseID<UAISense_Hearing>())
		return;
	OnReceiveNewStimulus(Actor, Stimulus);
}

void ASchismAIController::OnBestActorAlertDataUpdate()
{

	m_BestActorAlertData = &m_DefaultAlertData;
	m_BestActorOfInterest = nullptr;
	for (TMap<TObjectKey<AActor>, FActorAlertData>::TIterator IterAlertData = m_AudioAlertData.CreateIterator(); IterAlertData; ++IterAlertData)
	{
		TryExchangeAlertData(m_BestActorOfInterest, m_BestActorAlertData, IterAlertData->Key.ResolveObjectPtr(),IterAlertData.Value());
	}

	for (TMap<TObjectKey<AActor>, FActorAlertData>::TIterator IterAlertData = m_VisualAlertData.CreateIterator(); IterAlertData; ++IterAlertData)
	{
		TryExchangeAlertData(m_BestActorOfInterest, m_BestActorAlertData, IterAlertData.Key().ResolveObjectPtr(),IterAlertData.Value());
	}

	if (AIPerceptionDebug.GetValueOnGameThread())
	{
	}
	
	if (m_BestActorAlertData->CurrentAlertLevel < EAIAlertLevel::DETECTION)
		return;

	const uint8 alertLevelThreshold = static_cast<uint8>(m_BestActorAlertData->CurrentAlertLevel);
	
	for (uint8 i = 1; i <= alertLevelThreshold; i++)
		m_LastActorDetectionLocations.Emplace(static_cast<EAIAlertLevel>(i), m_BestActorAlertData->EventLocation);
}

