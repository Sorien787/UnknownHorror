// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Common/ListenerUtils.h"
#include "CoreMinimal.h"
#include "optional"
#include "Components/ActorComponent.h"
#include "ItemControlComponent.generated.h"


class IItemControlRequester
{
	bool m_bHasItemControl = false;
	bool m_bHasRequestedItemControl = false;
	AActor* m_pControllingActor = nullptr;
public:
	IItemControlRequester() {}
	virtual ~IItemControlRequester() {}

	UFUNCTION(BlueprintNativeEvent)
	void OnItemControlGranted(AActor* pControlledActor);
	virtual void OnItemControlGranted_Implementation(AActor* pControlledActor);

	UFUNCTION(BlueprintNativeEvent)
	void OnItemControlLost(AActor* pControlledActor);
	virtual void OnItemControlLost_Implementation(AActor* pControlledActor);
	
	virtual int GetPriority() const = 0;
	
	std::optional<AActor*> GetItemControl() const;
	
protected:
	bool RequestControlFromActor(AActor* pActor);
	bool RelinquishControlFromActor(AActor* pActor);

};

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
public:
	AItemActorBase();

	virtual void BeginPlay() override;

	virtual int GetPriority() const override;
protected:
	UPROPERTY(EditAnywhere)
	int m_ItemPriority;

	UPROPERTY(EditAnywhere)
	UItemControlComponent* m_pItemControlComponent;
};

// UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
// class DEEPSEAHORROR_API UItemComponentBase : public UActorComponent, public IItemControlRequester
// {
// 	GENERATED_BODY()
//
// 	
// };