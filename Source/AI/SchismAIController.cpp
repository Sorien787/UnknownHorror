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

static TAutoConsoleVariable<int32> AIPerceptionDebug(
	TEXT("AIPerceptionDebug"),
	0,
	TEXT("Shows AI Perception debug information.\n"),
	ECVF_Scalability | ECVF_RenderThreadSafe);


// states:
// Idle
// Roaming
// Searching
// and then we have the funky states:
// Chasing
// Skulking
// Stalking

// Idle -> Roaming -> Searching -> Other
// Based on current alert state

// Stalk enter state:
// Middle tension, Detection maxed
// We stay in stalking state when:
// We can perceive our quarry (Detection), and we're not perceived (not being looked at)
// Or we get to where we last perceived our quarry (< Detection, last Detection location)
// So we need a "Get last detection location" I suppose
// Exit state: Searching ( if we get to that spot) or Chasing (if we're otherwise triggered)

// Nodes:
// Get Last Detection Location
// Get Time Noticed Observed By Prey (= Time Observed By Prey, Time Detecting)
// Get Time Since Observing Prey ( Use Current Time, Time Detecting)
// Get Time Since Noticed Observed By Prey ( Use Current Time, Time Observed By Prey, Time Detecting)
// Used in Behaviour Tree to determine when we leave Skulk, etc.

// Skulk enter State:
// Low Tension, Detection Maxed (Or initiated directly by tension system)
// Prioritize corners
// and doorways
// maybe can use colliders to designate locations? so, player enters one collider, sets others active for skulk locations
// or maybe do it on a room-basis thing: player enters room A, sets X locations active
// We stay in Skulking state when:
// We can perceive our quarry (Detection) and we're not perceived for X seconds (based on tension)
// Otherwise, X seconds after we've lost Detection
// Exit state: Investigate, where we lost detection (Last Detection Loc)
// Chase:
// Escape:

// Chase Enter State:
// High Tension, Detection Maxed
// We stay in chase state:
// We perceive our quarry
// We get to where we last perceived our quarry (and it's not there)
// Exit state:
// Searching, or Attack

// after Investigate,
// run task for SetNotInterestedInObject, and use the Actor we're perceiving.

// based on Blackboard Nodes:
// do some internal logic as
// UpdateSkulking (EQS to SkulkLocation? Needs to be in the dark, line of sight, etc.)
// this one is annoying
// if we're observed, then make a task, after IsObservedForThreshold, - call - to either Escape or Chase
// if we're not observed, and we lose sight, go to Escape

// and UpdateSneaking
// if we're near the location, we can't be sneaking closer anymore - sets up to Chasing immediately
// if we're seen, set to Chasing immediately
// if we lose sight for X seconds, move to Investigating instead

// and UpdateChasing
// if we've reached a location, we can't be chasing anymore - set alerts down to Investigating immediately
// Or, if we're still chasing, that's fine - run in loop

//////////////////////////////////////////////////////////////////////////////////////////
void ASchismAIController::BeginPlay()
{
	Super::BeginPlay();
	RunBehaviorTree(m_pBTAsset);
	PrimaryActorTick.bCanEverTick = true;
	SetActorTickEnabled(true);
	m_pMyPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &ASchismAIController::OnTargetPerceptionUpdate);
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
	
	float perceptionAudioInterestScalar = 1;
	if(m_AudioAlertData.Contains(Actor))
	{
		perceptionAudioInterestScalar = m_ImportanceSensitivityScalar.EditorCurveData.Eval(m_AudioAlertData[Actor].CurrentPerceptionStrength);
	}
	if (GEngine && AIPerceptionDebug.GetValueOnGameThread())
		GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Yellow, FString::Printf(TEXT("PerceptionAudioInterestScalar: %f"), perceptionAudioInterestScalar));
	if (Stimulus.Type == UAISense::GetSenseID<UAISense_Sight>())
	{
		const float perceptionGeneralScalar = pPerceptionTypeComponent->GetVisualPerceptionModifier();
		const float perceptionDistanceScalar = pPerceptionTypeComponent->GetVisualPerceptionDistanceScalar(OffsetFromSource.Length());

		const float fieldOfViewDegrees = m_pSightConfig->PeripheralVisionAngleDegrees;
		const float angularOffsetDegrees = FMath::RadiansToDegrees(UnrealUtilities::GetRadAngleBetweenVectors(PerceptionSourceRotation.Quaternion().GetForwardVector(), OffsetFromSource));
		const float perceptionAngleScalar =  m_AngleFromCenterToVisualFalloffModifier.EditorCurveData.Eval(angularOffsetDegrees / fieldOfViewDegrees); 
		stimulusStrength *= perceptionDistanceScalar * perceptionGeneralScalar * perceptionAngleScalar * perceptionAudioInterestScalar;
		stimulusStrength = FMath::Min(stimulusStrength, pPerceptionTypeComponent->GetMaximumInterest());
		
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
			m_VisualAlertData[Actor].EventLocation = Stimulus.StimulusLocation;
		}
		
	}
	else if (Stimulus.Type == UAISense::GetSenseID<UAISense_Hearing>())
	{
		const float perceptionDistanceScalar = pPerceptionTypeComponent->GetAudioPerceptionDistanceScalar(OffsetFromSource.Length());
		stimulusStrength *= perceptionDistanceScalar * perceptionAudioInterestScalar;
		stimulusStrength = FMath::Min(stimulusStrength, pPerceptionTypeComponent->GetMaximumInterest());
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
			stimulusStrength = FMath::Min(perceptionAudioInterestScalar * stimulusStrength, currentAlertData.pPerceptionComponent->GetMaximumInterest());
			
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
	OnBlackboardVarsUpdate();
}

//////////////////////////////////////////////////////////////////////////////////////////
void ASchismAIController::DebugDrawAlertData(const FActorAlertData& AlertData) const
{
	if (!AIPerceptionDebug.GetValueOnGameThread())
		return;
	// remove the alert data since it's not really relevant anymore.
	
	DrawDebugString(GetWorld(), AlertData.EventLocation, *FString::Printf(TEXT("Current perception strength: %f \n Maximum perception: %f\n Maximum alert level: %s\n Current alert level: %s\n Priority: %i\n"),
		AlertData.CurrentPerceptionStrength,
		AlertData.pPerceptionComponent->GetMaximumInterest(),
		*StaticEnum<EAIAlertLevel>()->GetValueAsString(AlertData.pPerceptionComponent->GetMaximumAlertLevel()),
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
				return;
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
void ASchismAIController::OnBlackboardVarsUpdate()
{
	Blackboard->SetValueAsVector(MostInterestingLocationBlackboardKey, m_BestActorOfInterest ? m_BestActorOfInterest->GetActorLocation() : FVector::ZeroVector);
	Blackboard->SetValueAsObject(MostInterestingActorBlackboardKey, m_BestActorOfInterest);
	Blackboard->SetValueAsEnum(AlertLevelBlackboardKey, static_cast<uint8>(m_BestActorAlertData->CurrentAlertLevel));
	Blackboard->SetValueAsBool(HasVisuallyDetectedPlayerBlackboardKey, m_bActorVisible);
	const float searchRadius = m_SearchPrecisionByCurrentAlertness.EditorCurveData.Eval(m_BestActorAlertData->CurrentPerceptionStrength);
	Blackboard->SetValueAsFloat( RelevantSearchRadiusBlackboardKey, searchRadius);
	const float searchProbability = m_SearchProbabilityByCurrentAlertness.EditorCurveData.Eval(m_BestActorAlertData->CurrentPerceptionStrength);
	Blackboard->SetValueAsFloat( SearchProbabilityByPerceptionBlackboardKey, searchProbability);

	if (!AIPerceptionDebug.GetValueOnGameThread())
		return;

	FString valA = m_bActorVisible ? "Yes" : "No";
	FString valB = m_BestActorOfInterest ? m_BestActorOfInterest->GetName() : "Invalid";
	FString str = FString::Printf(
		TEXT("MostInterestingActorBlackboardKey: %s, \n Current Alert Level: %s \n Has Visually Detected Most Interesting Actor: %s \n Relevant Search Radius: %f \n Search Probability: %f"),
		*valB,
		*UEnum::GetValueAsString(m_BestActorAlertData->CurrentAlertLevel),
		*valA,
		searchRadius,
		searchProbability
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
	float decayDelay = 0;
	if (AlertData.SenseID == UAISense::GetSenseID<UAISense_Sight>())
	{
		decayDelay = AlertData.pPerceptionComponent->GetVisualDecayDelay();
	}
	else if (AlertData.SenseID == UAISense::GetSenseID<UAISense_Hearing>())
	{
		decayDelay = AlertData.pPerceptionComponent->GetAudioDecayDelay();
	}
	if (decayDelay < GetWorld()->GetTimeSeconds() -  AlertData.EventTime)
	{
		AlertData.CurrentPerceptionStrength -= DeltaTime * AlertData.pPerceptionComponent->GetPerceptionDecay(AlertData.CurrentPerceptionStrength / AlertData.pPerceptionComponent->GetMaximumInterest());
		AlertData.CurrentPerceptionStrength = FMath::Max(0.0f, AlertData.CurrentPerceptionStrength);
	}

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
		const EAIAlertLevel maximumAlertLevel = AlertData.pPerceptionComponent->GetMaximumAlertLevel();
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
void ASchismAIController::TryExchangeAlertData(AActor* pBestActor, ISharedActorAlertData* bestActorAlertData, AActor* pNewActor, FActorAlertData& newActorAlertData) const
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


void ASchismAIController::LoseInterestInActor(AActor* pActor)
{
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

	if (m_BestActorAlertData->CurrentAlertLevel < EAIAlertLevel::DETECTION)
		return;

	const uint8 alertLevelThreshold = static_cast<uint8>(m_BestActorAlertData->CurrentAlertLevel);
	
	for (uint8 i = 1; i <= alertLevelThreshold; i++)
		m_LastActorDetectionLocations[static_cast<EAIAlertLevel>(i)] = m_BestActorAlertData->EventLocation;
}

