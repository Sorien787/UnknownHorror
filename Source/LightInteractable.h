// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/PointLightComponent.h"
#include "Components/WidgetComponent.h"
#include "InteractableObjectBase.h"
#include "LightModifierComponent.h"
#include "LightInteractable.generated.h"

UCLASS()
class DEEPSEAHORROR_API ALightInteractable : public AInteractableObjectBase
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALightInteractable();

	virtual void OnInteractionStarted(UInteractionUserComponent* pInteractable, int interactorId) override;

	virtual bool IsInteractionAvailable(const UInteractionUserComponent* pInteractable, int interactorId) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* m_LightSwitchMesh;

	UPROPERTY(EditAnywhere)
		UPointLightComponent* m_Light;

	UPROPERTY(EditAnywhere)
		ULightModifierComponent* m_LightModifier;

	UPROPERTY(EditAnywhere)
		float m_LightOnIntensity;

	void SetLightStateBasedOnData();
	
	bool m_IsSwitchOn{ false };
};
