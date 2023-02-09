// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemControl/ItemControlComponent.h"
#include <algorithm>
#include "Common/UnrealUtilities.h"

bool IItemControlRequester::RequestControlFromActor(AActor* pActor)
{
	check(!m_bHasRequestedItemControl)
	m_bHasRequestedItemControl = true;
	return UnrealUtilities::GetComponentFromActor<UItemControlComponent>(pActor)->RequestItemControl(this);
}

bool IItemControlRequester::RelinquishControlFromActor(AActor* pActor)
{
	check(m_bHasRequestedItemControl)
	m_bHasRequestedItemControl = false;
	return UnrealUtilities::GetComponentFromActor<UItemControlComponent>(pActor)->ReleaseItemControl(this);
}

void IItemControlRequester::OnItemControlGranted_Implementation(AActor* pControlledActor)
{
	check(m_pControllingActor == nullptr)
	m_bHasItemControl = true;
	m_pControllingActor = pControlledActor;
}

void IItemControlRequester::OnItemControlLost_Implementation(AActor* pControlledActor)
{
	check(pControlledActor == pControlledActor)
	m_bHasItemControl = false;
	m_pControllingActor = nullptr;
}

std::optional<AActor*> IItemControlRequester::GetItemControl() const
{
	if (!m_bHasItemControl)
		return std::optional<AActor*>();
	return std::optional<AActor*>();
}


// Called when the game starts
void UItemControlComponent::BeginPlay()
{
	Super::BeginPlay();
}

bool UItemControlComponent::RequestItemControl(IItemControlRequester* requester)
{
	IItemControlRequester* currentControl =  m_pendingItemControlRequests.back();
	
	// item control already requested
	const auto it = std::find(m_pendingItemControlRequests.begin(), m_pendingItemControlRequests.end(), requester);
	if (it == m_pendingItemControlRequests.end())
		return false;
	
	m_pendingItemControlRequests.push_back(requester);
	std::sort(m_pendingItemControlRequests.begin(), m_pendingItemControlRequests.end(), [](IItemControlRequester* left, IItemControlRequester* right)
	{
		return left->GetPriority() > right->GetPriority();
	});
	
	// maintained control as before
	if (currentControl == m_pendingItemControlRequests.back())
		return requester == currentControl;

	// new control is different (somehow) but not our new requester.
	if (requester != m_pendingItemControlRequests.back())
		return false;
	currentControl->OnItemControlLost(GetOwner());
	m_pendingItemControlRequests.back()->OnItemControlGranted(GetOwner());
	return true;
}

bool UItemControlComponent::ReleaseItemControl(IItemControlRequester* requester)
{
	const IItemControlRequester* currentControl = m_pendingItemControlRequests.back();
	const auto it = std::find(m_pendingItemControlRequests.begin(), m_pendingItemControlRequests.end(), requester);
	if (it == m_pendingItemControlRequests.end())
		return false;
	// item control wasnt requested
	m_pendingItemControlRequests.erase(it);
	
	if (currentControl != requester)
		return false;
	
	requester->OnItemControlLost(GetOwner());
	m_pendingItemControlRequests.back()->OnItemControlGranted(GetOwner());
	return true;
}

bool UItemControlComponent::IsItemControlledByRequester(const IItemControlRequester* requester) const
{
	return m_pendingItemControlRequests.back() == requester;
}

bool UItemControlComponent::WouldItemControlBeGainedByRequester(const IItemControlRequester* requester) const
{
	return requester->GetPriority() > m_pendingItemControlRequests.back()->GetPriority();
}

AItemActorBase::AItemActorBase()
{
	m_pItemControlComponent = CreateDefaultSubobject<UItemControlComponent>(TEXT("Item Control Component"));
	check(m_pItemControlComponent != nullptr)
}

void AItemActorBase::BeginPlay()
{
	Super::BeginPlay();
	m_pItemControlComponent->RequestItemControl(this);
}

int AItemActorBase::GetPriority() const
{
	return m_ItemPriority;
}

