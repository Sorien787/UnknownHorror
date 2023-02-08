// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Common/ListenerUtils.h"
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ItemControlComponent.generated.h"


class IItemControlRequester
{
	bool m_bHasItemControl = false;

public:
	virtual void OnItemControlGranted() = 0;
	virtual void OnItemControlLost() = 0;
	virtual int GetPriority() const = 0;
	
	bool RequestControlFromActor(AActor* pActor);
	bool HasItemControl() const;

	IItemControlRequester() {}
	virtual ~IItemControlRequester() {}
};

class DefaultItemControl : public IItemControlRequester
{
	virtual void OnItemControlGranted() override{};
	virtual void OnItemControlLost() override {};
	virtual int GetPriority() const override {return 0;}
};


static constexpr DefaultItemControl m_sDefaultItemControl = DefaultItemControl();

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DEEPSEAHORROR_API UItemControlComponent : public UActorComponent
{
	GENERATED_BODY()
	
	std::vector<IItemControlRequester*> m_pendingItemControlRequests;

public:	
	virtual void BeginPlay() override;
	
	bool RequestItemControl(IItemControlRequester* requester);

	bool ReleaseItemControl(IItemControlRequester* requester);

	bool IsItemControlledByRequester(const IItemControlRequester* requester) const;

	bool WouldItemControlBeGainedByRequester(const IItemControlRequester* requester) const;
};


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DEEPSEAHORROR_API AItemActorBase : public AActor, public IItemControlRequester
{
	GENERATED_BODY()

	AItemActorBase();

	virtual void BeginPlay() override;

	virtual int GetPriority() const override;

	UPROPERTY(EditAnywhere)
	int m_ItemPriority;

	UPROPERTY(EditAnywhere)
	UItemControlComponent* m_pItemControlComponent;
};
