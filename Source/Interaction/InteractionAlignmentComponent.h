// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "InteractionAlignmentComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DEEPSEAHORROR_API UInteractionAlignmentComponent : public USceneComponent
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	int m_InteractionID;

public:	

	virtual int GetInteractorId() const {return m_InteractionID;}
		
};
