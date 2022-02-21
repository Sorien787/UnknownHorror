// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/PointLightComponent.h"
#include "Components/WidgetComponent.h"
#include "InteractableInterface.h"
#include "LightModifierComponent.h"
#include "LightInteractable.generated.h"

UCLASS()
class DEEPSEAHORROR_API ALightInteractable 
	: public AActor
	, public IInteractableInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALightInteractable();

	virtual void OnInteractionStarted(UInteractionUserComponent* pInteractable) override;

	virtual bool IsInteractionAvailable(const UInteractionUserComponent* pInteractable, OUT int type) const override;

	virtual void OnShowInteractionWidget() override;

	virtual void OnHideInteractionWidget() override;

	virtual FVector GetCurrentLocation() const override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(EditAnywhere)
		USceneComponent* m_RootComponent;

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* m_LightSwitchMesh;

	UPROPERTY(EditAnywhere)
		UPointLightComponent* m_Light;

	UPROPERTY(EditAnywhere)
		ULightModifierComponent* m_LightModifier;

	UPROPERTY(EditAnywhere)
		UWidgetComponent* m_pInteractionWidget;

	UPROPERTY(EditAnywhere)
		float m_LightOnIntensity;

	void SetLightStateBasedOnData();
	
	bool m_IsSwitchOn{ false };
};
