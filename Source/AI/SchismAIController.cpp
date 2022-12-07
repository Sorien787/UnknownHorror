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
// We stay in Skulking state when:
// We can perceive our quarry (Detection) and we're not perceived for X seconds (based on tension)
// Otherwise, X seconds after we've lost Detection
// Exit state:

// Chase Enter State:
// High Tension, Detection Maxed
// We stay in chase state:
// We perceive our quarry
// We get to where we last perceived our quarry (and it's not there)
// Exit state:
// Searching, or Attack

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
	
	if (Stimulus.Type == UAISense::GetSenseID<UAISense_Sight>())
	{
		float perceptionAudioInterestScalar = 1;
		if(m_AudioAlertData.Contains(Actor))
		{
			float TotalAudioInterest = 0;
			const TArray<FActorAlertData>& AudioAlertData = m_AudioAlertData[Actor];
			for(size_t nAudioIndex = 0; nAudioIndex < AudioAlertData.Num(); nAudioIndex++)
			{
				TotalAudioInterest += AudioAlertData[nAudioIndex].CurrentPerceptionStrength;
			}
			perceptionAudioInterestScalar = m_TotalAudioInterestToVisualScalar.EditorCurveData.Eval(TotalAudioInterest);
		}
	

		const float perceptionGeneralScalar = pPerceptionTypeComponent->GetVisualPerceptionModifier();
		const float perceptionDistanceScalar = pPerceptionTypeComponent->GetVisualPerceptionDistanceScalar(OffsetFromSource.Length());

		const float fieldOfViewDegrees = m_pSightConfig->PeripheralVisionAngleDegrees;
		const float angularOffsetDegrees = FMath::RadiansToDegrees(UnrealUtilities::GetRadAngleBetweenVectors(PerceptionSourceRotation.Quaternion().GetForwardVector(), OffsetFromSource));
		const float perceptionAngleScalar =  m_AngleFromCenterToVisualFalloffModifier.EditorCurveData.Eval(angularOffsetDegrees / fieldOfViewDegrees); 
		stimulusStrength *= perceptionDistanceScalar * perceptionGeneralScalar * perceptionAngleScalar * perceptionAudioInterestScalar;
		stimulusStrength = FMath::Min(stimulusStrength, pPerceptionTypeComponent->GetMaximumInterest());
		if(!m_VisualAlertData.Contains(Actor))
		{
			m_VisualAlertData.Add(Actor,FActorAlertData(pPerceptionTypeComponent, Stimulus.Type, stimulusStrength, GetWorld()->GetTimeSeconds(),Stimulus.StimulusLocation));
		}
		else
		{
			m_VisualAlertData[Actor].EventTimeoutValue = 0.0f;
			m_VisualAlertData[Actor].CurrentPerceptionStrength += stimulusStrength;
			m_VisualAlertData[Actor].EventLocation = Stimulus.StimulusLocation;
			m_VisualAlertData[Actor].CurrentPerceptionStrength = FMath::Min(m_VisualAlertData[Actor].CurrentPerceptionStrength, m_VisualAlertData[Actor].pPerceptionComponent->GetMaximumInterest());
		}
		
	}
	else if (Stimulus.Type == UAISense::GetSenseID<UAISense_Hearing>())
	{
		const float perceptionDistanceScalar = pPerceptionTypeComponent->GetAudioPerceptionDistanceScalar(OffsetFromSource.Length());
		stimulusStrength *= perceptionDistanceScalar;
		stimulusStrength = FMath::Min(stimulusStrength, pPerceptionTypeComponent->GetMaximumInterest());
		if (!m_AudioAlertData.Contains(Actor))
		{
			m_AudioAlertData.Add(Actor, TArray<FActorAlertData>());
		}
		m_AudioAlertData[Actor].Emplace(pPerceptionTypeComponent, Stimulus.Type,stimulusStrength, GetWorld()->GetTimeSeconds(), Stimulus.StimulusLocation);
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
		AlertData.pPerceptionComponent->GetPriority()), nullptr, FColor::White, 0.05f);
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
	for (TMap<TObjectKey<AActor>, TArray<FActorAlertData>>::TIterator Iter = m_AudioAlertData.CreateIterator(); Iter; ++Iter)
	{
		for (TArray<FActorAlertData>::TIterator IterAlertData = Iter->Value.CreateIterator(); IterAlertData; ++IterAlertData)
		{
			TickAlertData(*IterAlertData, DeltaSeconds);
			DebugDrawAlertData(*IterAlertData);
			
			if (!ShouldRemovePerceptionData(*IterAlertData))
				continue;
			
			IterAlertData.RemoveCurrent();
		}
		if (Iter.Value().IsEmpty())
			Iter.RemoveCurrent();
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
	Blackboard->SetValueAsVector(TargetLocationBlackboardKey, m_BestActorOfInterest ? m_BestActorOfInterest->GetActorLocation() : FVector::ZeroVector);
	Blackboard->SetValueAsEnum(AlertLevelBlackboardKey, static_cast<uint8>(m_BestActorAlertData->CurrentAlertLevel));
	Blackboard->SetValueAsBool(HasVisuallyDetectedPlayerBlackboardKey, m_bActorVisible);
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
void ASchismAIController::TryExchangeAlertData(const AActor* pBestActor, const ISharedActorAlertData* bestActorAlertData, const AActor* pNewActor, const FActorAlertData& newActorAlertData)
{
	// pick via alert level
	if (bestActorAlertData->CurrentAlertLevel > newActorAlertData.CurrentAlertLevel)
		return;
	// and then by perception strength
	if (bestActorAlertData->CurrentAlertLevel == newActorAlertData.CurrentAlertLevel && bestActorAlertData->CurrentPerceptionStrength > newActorAlertData.CurrentPerceptionStrength)
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

void ASchismAIController::LoseInterestInActor(AActor* pActor)
{
	m_LostInterestSet.Add(pActor);
	for (TMap<TObjectKey<AActor>, TArray<FActorAlertData>>::TIterator Iter = m_AudioAlertData.CreateIterator(); Iter; ++Iter)
	{
		if (Iter.Key() != pActor)
			continue;
		Iter.RemoveCurrent();
		break;
	}
	
	for (TMap<TObjectKey<AActor>, FActorAlertData>::TIterator IterAlertData = m_VisualAlertData.CreateIterator(); IterAlertData; ++IterAlertData)
	{
		if (IterAlertData.Key() != pActor)
			continue;
		IterAlertData.RemoveCurrent();
		break;
	}
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
	bool isActorOfInterestVisible = false;
	AActor* initialBestActor = m_BestActorOfInterest;
	m_BestActorAlertData = &m_DefaultAlertData;
	m_BestActorOfInterest = nullptr;
	for (TMap<TObjectKey<AActor>, TArray<FActorAlertData>>::TIterator Iter = m_AudioAlertData.CreateIterator(); Iter; ++Iter)
	{
		for (TArray<FActorAlertData>::TIterator IterAlertData = Iter->Value.CreateIterator(); IterAlertData; ++IterAlertData)
		{
			TryExchangeAlertData(m_BestActorOfInterest, m_BestActorAlertData, Iter->Key.ResolveObjectPtr(),*IterAlertData);
		}
	}

	for (TMap<TObjectKey<AActor>, FActorAlertData>::TIterator IterAlertData = m_VisualAlertData.CreateIterator(); IterAlertData; ++IterAlertData)
	{
		TryExchangeAlertData(m_BestActorOfInterest, m_BestActorAlertData, IterAlertData.Key().ResolveObjectPtr(),IterAlertData.Value());
	}
}

