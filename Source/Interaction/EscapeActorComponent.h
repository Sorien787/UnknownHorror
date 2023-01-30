// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EscapeActorComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DEEPSEAHORROR_API UEscapeActorComponent : public UActorComponent
{
	GENERATED_BODY()
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	UFUNCTION()
	void ActivateLocation();

	UFUNCTION()
	void DeactivateLocation();

	bool GetIsActive() const;

	bool m_IsActive = true;
	
	UPROPERTY(EditAnywhere)
	bool m_DefaultActive = true;
	
};