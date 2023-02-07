// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemControlComponent.h"
#include "Components/ActorComponent.h"
#include "ItemControllerComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DEEPSEAHORROR_API UItemControllerComponent : public UActorComponent, public IItemControlRequester
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UItemControllerComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
};
