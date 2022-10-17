// Fill out your copyright notice in the Description page of Project Settings.


#include "Lighting/TrackerPointLight.h"
#include "Common/UnrealUtilities.h"
#include "Gameplay/LightModifierComponent.h"

bool ATrackerPointLight::IsProvidingEnoughLight() const
{
	return m_pPointLight->Intensity > m_MinIntensityToBeConsideredOn;
}

void ATrackerPointLight::OnSetLightIntensity(float lightIntensity)
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

// Sets default values
ATrackerPointLight::ATrackerPointLight()
{
	m_pPointLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("Light"));
	check(m_pPointLight != nullptr);
	m_pPointLight->SetupAttachment(GetRootComponent());

	m_pCollisionShape = CreateDefaultSubobject<USphereComponent>(TEXT("Collision Area"));
	check(m_pCollisionShape != nullptr);
	m_pCollisionShape->SetupAttachment(GetRootComponent());

	m_pLightModifier = CreateDefaultSubobject<ULightModifierComponent>(TEXT("Light Modifier"));
	check(m_pLightModifier != nullptr);
}

// Called when the game starts or when spawned
void ATrackerPointLight::BeginPlay()
{
	Super::BeginPlay();

	m_pCollisionShape->SetSphereRadius(m_pPointLight->AttenuationRadius);

	m_pCollisionShape->OnComponentBeginOverlap.AddDynamic(this, &ATrackerPointLight::OnBoxBeginOverlap);
	m_pCollisionShape->OnComponentBeginOverlap.AddDynamic(this, &ATrackerPointLight::OnBoxBeginOverlap);

	m_pLightModifier->m_LightIntensityDelegate.AddDynamic(this, &ATrackerPointLight::OnSetLightIntensity);
	OnSetLightIntensity(m_pPointLight->Intensity);
}

void ATrackerPointLight::OnBoxEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
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

void ATrackerPointLight::OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ULightSensitivityComponent* pLightSensitiveComponent = UnrealUtilities::GetComponentFromActor<ULightSensitivityComponent>(OtherActor);

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


