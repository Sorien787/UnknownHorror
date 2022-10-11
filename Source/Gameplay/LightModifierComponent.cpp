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
	
	m_IsFlickering = false;
	m_InstancedMat = UMaterialInstanceDynamic::Create(Material, this);
	
	if (m_DefaultOn)
		SwitchOn(true);
	else
		SwitchOff(true);

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
		
		Meshes[i]->SetMaterial(0, m_InstancedMat);
		
		hasFoundBulbMesh = true;
		
		AddMeshToControlGroup(Meshes[i]);		
	}
	
	if (hasFoundBulbMesh)
		return;

	UE_LOG(LogTemp, Warning, TEXT("Light Modifier Component %s found Meshes, but no mesh had the name 'BulbMesh', is this intentional?"), *GetOwner()->GetName());
}

void ULightModifierComponent::SwitchOn(bool force /* = false */)
{
	if(m_IsOn && !force)
		return;
	m_IsOn = true;
	SetComponentTickEnabled(true);
	SetLightIntensity(1.0f);
}

void ULightModifierComponent::SwitchOff(bool force /* = false */)
{
	if (!m_IsOn && !force)
		return;
	m_IsOn = false;
	SetComponentTickEnabled(false);
	SetLightIntensity(0.0f);
	m_IsFlickering = false;
}

void ULightModifierComponent::SetLightIntensity(float intensity)
{
	m_LightIntensityDelegate.Broadcast(intensity);
	m_CurrentIntensity = intensity;
	for (auto pLightComponent : m_pLightComponentsArray)
	{
		pLightComponent->SetIntensity(intensity * m_DefaultBrightness);
	}
	m_InstancedMat->SetScalarParameterValue("Emissive_Strength", m_LightIntensityToEmissivity.EditorCurveData.Eval(intensity));
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

void ULightModifierComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	float transitionAttemptProbability;

	// here's the problem:

	// want to have a frequency (ish)

	// so maybe 1/frequency is period

	// and scale the transition probability by (time since last on) / (period) = (time since last on) * frequency

	// so that we're less likely to transition at a fequency lower than desired

	// and mroe likely to at a frequency higher than desired

	if (m_IsFlickering)
		transitionAttemptProbability = FMath::FRandRange(0.0f, m_LightDesiredPercentOnline);
	else
		transitionAttemptProbability = FMath::FRandRange(0.0f, 1-m_LightDesiredPercentOnline);
	
	if(transitionAttemptProbability < FMath::FRandRange(0.0f, 1.0f))
		return;
	// m_LightFlickerFrequency

	m_IsFlickering = !m_IsFlickering;
	
	m_LightFlickerDelegate.Broadcast(m_IsFlickering);
	m_LightIntensityDelegate.Broadcast(m_CurrentIntensity);
	
	if (m_IsFlickering)
	{

		SetLightIntensity(FMath::FRandRange(m_MinFlickerBrightness, m_MaxFlickerBrightness));	
	}
	else
	{
		SetLightIntensity(1.0f);
	}
}

