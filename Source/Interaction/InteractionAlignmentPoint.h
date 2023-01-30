// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractionAlignmentPoint.generated.h"

UCLASS()
class DEEPSEAHORROR_API UInteractionAlignmentPointComponent : public USceneComponent
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	int m_InteractionID;

public:	

	virtual int GetInteractorId() const {return m_InteractionID;}
};
