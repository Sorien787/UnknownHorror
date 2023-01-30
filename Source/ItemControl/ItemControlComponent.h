// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ItemControlComponent.generated.h"


class IItemControlRequester
{
	virtual void OnItemControlGranted() = 0;
	virtual void OnItemControlLost() = 0;

	int priority = 0;
};

class DefaultItemControl : IItemControlRequester
{
	virtual void OnItemControlGranted() override{};
	virtual void OnItemControlLost() override {};
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DEEPSEAHORROR_API UItemControlComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UItemControlComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	std::vector<IItemControlRequester> m_pendingItemControlRequests;
	
	bool RequestItemControl(IItemControlRequester* requester);

	bool ReleaseItemControl(IItemControlRequester* requester);

	bool IsItemControlledByRequester(const IItemControlRequester* requester) const;
};
