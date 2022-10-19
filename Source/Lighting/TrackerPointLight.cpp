// Fill out your copyright notice in the Description page of Project Settings.


#include "Lighting/TrackerPointLight.h"
#include "Common/UnrealUtilities.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "Gameplay/LightModifierComponent.h"
#pragma optimize("", off)
bool UTrackerPointLight::IsProvidingEnoughLight() const
{
	return m_pPointLight->Intensity > m_MinIntensityToBeConsideredOn;
}

void UTrackerPointLight::OnActorOverlap(AActor* pActor)
{
ULightSensitivityComponent* pLightSensitiveComponent = UnrealUtilities::GetComponentFromActor<ULightSensitivityComponent>(pActor);

	if (!pLightSensitiveComponent)
		return;

	const auto& it = std::find(m_pLightSensitivityComponents.begin(), m_pLightSensitivityComponents.end(), pLightSensitiveComponent);

	if (it != m_pLightSensitivityComponents.end())
		return;
	
	m_pLightSensitivityComponents.push_back(pLightSensitiveComponent);

	if (!IsProvidingEnoughLight())
		return;
	
	pLightSensitiveComponent->OnEnterLightArea();
}

void UTrackerPointLight::OnSetLightIntensity(float lightIntensity)
{
	const bool bIsNowProvidingEnoughLight = IsProvidingEnoughLight();

	if (m_bIsBrightEnough == bIsNowProvidingEnoughLight)
		return;

	m_bIsBrightEnough = bIsNowProvidingEnoughLight;

	if (m_bIsBrightEnough)
	{
		for (size_t i = 0; i < m_pLightSensitivityComponents.size(); i++)
		{
			ULightSensitivityComponent* pLightSensitivityComponent = m_pLightSensitivityComponents[i];
			pLightSensitivityComponent->OnEnterLightArea();
		}
	}
	else
	{
		for (size_t i = 0; i < m_pLightSensitivityComponents.size(); i++)
		{
			ULightSensitivityComponent* pLightSensitivityComponent = m_pLightSensitivityComponents[i];
			pLightSensitivityComponent->OnExitLightArea();
		}
	}
}

void UTrackerPointLight::BeginPlay()
{
	Super::BeginPlay();
	
	USphereComponent* pCollisionShape = UnrealUtilities::GetComponentFromActor<USphereComponent>(GetOwner());
	ULightModifierComponent* pLightModifierComponent = UnrealUtilities::GetComponentFromActor<ULightModifierComponent>(GetOwner());
	m_pPointLight = UnrealUtilities::GetComponentFromActor<UPointLightComponent>(GetOwner());

	if (!pCollisionShape || !pLightModifierComponent || !m_pPointLight)
		return;

	m_bIsBrightEnough = IsProvidingEnoughLight();
	TArray<AActor*> pActors;
	pCollisionShape->GetOverlappingActors(pActors);
	for (size_t i = 0; i < pActors.Num(); i++)
	{
		OnActorOverlap(pActors[i]);
	}
	pCollisionShape->SetSphereRadius(m_pPointLight->AttenuationRadius);
	pCollisionShape->OnComponentBeginOverlap.AddDynamic(this, &UTrackerPointLight::OnBoxBeginOverlap);
	pCollisionShape->OnComponentEndOverlap.AddDynamic(this, &UTrackerPointLight::OnBoxEndOverlap);

	pLightModifierComponent->m_LightIntensityDelegate.AddDynamic(this, &UTrackerPointLight::OnSetLightIntensity);
}

void UTrackerPointLight::InitializeComponent()
{
	Super::InitializeComponent();


}

void UTrackerPointLight::OnBoxEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ULightSensitivityComponent* pLightSensitiveComponent = UnrealUtilities::GetComponentFromActor<ULightSensitivityComponent>(OtherActor);

	if (!pLightSensitiveComponent)
		return;

	const auto& it = std::find(m_pLightSensitivityComponents.begin(), m_pLightSensitivityComponents.end(), pLightSensitiveComponent);

	if (it == m_pLightSensitivityComponents.end())
		return;

	m_pLightSensitivityComponents.erase(it);
	
	if(!IsProvidingEnoughLight())
		return;

	pLightSensitiveComponent->OnExitLightArea();
}

void UTrackerPointLight::OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	OnActorOverlap(OtherActor);
}


