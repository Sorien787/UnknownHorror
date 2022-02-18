// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/LightComponent.h"
#include "LightModifierComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DEEPSEAHORROR_API ULightModifierComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	ULightModifierComponent();

	void AddLightToControlGroup(ULightComponent* pLightComponent);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY()
		TArray<ULightComponent*> m_pLightComponentsArray;
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
