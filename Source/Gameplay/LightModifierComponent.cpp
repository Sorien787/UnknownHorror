// Fill out your copyright notice in the Description page of Project Settings.


#include "LightModifierComponent.h"
#include "Components/PointLightComponent.h"

// Sets default values for this component's properties
ULightModifierComponent::ULightModifierComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void ULightModifierComponent::BeginPlay()
{	
	Super::BeginPlay();
	SetComponentTickEnabled(false);
	m_CurrentLightFlickerState = m_DefaultLightFlickerState;
	m_IsFlickering = false;
	m_InstanceBulbMat = UMaterialInstanceDynamic::Create(m_BulbMaterial, this);
	
	if (m_DefaultOn)
		PowerOn(true);
	else
		PowerOff(true);

	TArray<ULightComponent*> Lights;
	GetOwner()->GetComponents<ULightComponent>(Lights);
	
	for (int i = 0; i < Lights.Num(); i++)
	{
		AddLightToControlGroup(Lights[i]);		
	}

	TArray<UStaticMeshComponent*> Meshes;
	GetOwner()->GetComponents<UStaticMeshComponent>(Meshes);

	if (Meshes.Num() < 1)
		return;
	
	bool hasFoundBulbMesh = false;
	for (int i = 0; i < Meshes.Num(); i++)
	{
		if (!Meshes[i]->ComponentHasTag(FName("BulbMesh")))
			continue;
		
		Meshes[i]->SetMaterial(0, m_InstanceBulbMat);
		
		hasFoundBulbMesh = true;
		
		AddMeshToControlGroup(Meshes[i]);		
	}
	
	if (hasFoundBulbMesh)
		return;

	UE_LOG(LogTemp, Warning, TEXT("Light Modifier Component %s found Meshes, but no mesh had the tag 'BulbMesh', is this intentional?"), *GetOwner()->GetName());
}

void ULightModifierComponent::Break()
{
	m_IsBroken = true;
	PowerOff();
	SetIntensityScalar(m_BrightnessWhenBreaking);
	m_LightBreakDelegate.Broadcast();
	GetWorld()->GetTimerManager().SetTimer(m_BreakTimerHandle, this, &ULightModifierComponent::OnFinishedBreaking, m_LengthOfBreakFlash, false, m_LengthOfBreakFlash);
}

void ULightModifierComponent::SetFlickerStatusOverride(const FLightFlickerStateStruct& flickerStateOverride)
{
	m_CurrentLightFlickerState = flickerStateOverride;
}

void ULightModifierComponent::CancelFlickerStatusOverride()
{
	m_CurrentLightFlickerState = m_DefaultLightFlickerState;
}

void ULightModifierComponent::PowerOn(bool force /* = false */)
{
	if(m_IsOn && !force || m_IsBroken)
		return;
	m_IsOn = true;
	SetComponentTickEnabled(true);
	SetIntensityScalar(1.0f);
}

void ULightModifierComponent::PowerOff(bool force /* = false */)
{
	if (!m_IsOn && !force)
		return;
	m_IsOn = false;
	SetComponentTickEnabled(false);
	SetIntensityScalar(0.0f);
	m_IsFlickering = false;
}

void ULightModifierComponent::SetIntensityScalar(float intensity)
{
	if (!m_IsOn && intensity > 0.0f)
		return;
	m_LightIntensityDelegate.Broadcast(intensity);
	m_CurrentIntensity = intensity;
	for (auto pLightComponent : m_pLightComponentsArray)
	{
		pLightComponent->SetIntensity(intensity * m_DefaultBrightness);
	}
	const float desiredStrength = m_LightIntensityToEmissivity.EditorCurveData.Eval(intensity);
	m_InstanceBulbMat->SetScalarParameterValue("Emissive_Strength", desiredStrength );
}

void ULightModifierComponent::AddLightToControlGroup(ULightComponent* pLightComponent)
{
	m_pLightComponentsArray.Add(pLightComponent);
	pLightComponent->SetIntensity(m_CurrentIntensity * m_DefaultBrightness);
}

void ULightModifierComponent::AddMeshToControlGroup(UMeshComponent* pMeshComponent)
{
	m_pMeshComponentsArray.Add(pMeshComponent);
}

void ULightModifierComponent::OnFinishedBreaking()
{
	SetIntensityScalar(-10.0f);
}

void ULightModifierComponent::OnItemControlGranted_Implementation(AActor* pControlledActor)
{
	m_CurrentLightFlickerState = m_DefaultLightFlickerState;
}

void ULightModifierComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	float transitionAttemptProbability;

	if (m_IsFlickering)
		transitionAttemptProbability = FMath::FRandRange(0.0f, m_CurrentLightFlickerState.m_LightDesiredPercentOnline);
	else
		transitionAttemptProbability = FMath::FRandRange(0.0f, 1-m_CurrentLightFlickerState.m_LightDesiredPercentOnline);
	
	const float currentTime = GetWorld()->GetTimeSeconds();

	const float frequencyScalar = m_ApplyFrequencyScalar ? (currentTime - m_TimeLastStateTransition) * m_CurrentLightFlickerState.m_LightFlickerFrequency : 1.0f;

	transitionAttemptProbability *= frequencyScalar;
	
	if(transitionAttemptProbability < FMath::FRandRange(0.0f, 1.0f))
		return;

	m_IsFlickering = !m_IsFlickering;

	if (m_IsFlickering)
	{
		SetIntensityScalar(FMath::FRandRange(m_CurrentLightFlickerState.m_MinFlickerBrightness, m_CurrentLightFlickerState.m_MaxFlickerBrightness));	
	}
	else
	{
		SetIntensityScalar(1.0f);
	}
	
	m_LightFlickerDelegate.Broadcast(m_IsFlickering);
	m_LightIntensityDelegate.Broadcast(m_CurrentIntensity);

	m_TimeLastStateTransition = currentTime;
}

