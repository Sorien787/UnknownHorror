// Fill out your copyright notice in the Description page of Project Settings.

#include <algorithm>
#include "ItemControl/ItemControlComponent.h"

#include "Common/UnrealUtilities.h"

bool IItemControlRequester::RequestControlFromActor(AActor* pActor)
{
	return UnrealUtilities::GetComponentFromActor<UItemControlComponent>(pActor)->RequestItemControl(this);
}

bool IItemControlRequester::HasItemControl() const
{
	return m_bHasItemControl;
}


// Called when the game starts
void UItemControlComponent::BeginPlay()
{
	Super::BeginPlay();
	const IItemControlRequester* pDefault = (&m_sDefaultItemControl);
	m_pendingItemControlRequests.push_back(const_cast<IItemControlRequester*>(pDefault));
}

bool UItemControlComponent::RequestItemControl(IItemControlRequester* requester)
{
	IItemControlRequester* currentControl =  m_pendingItemControlRequests.back();
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
	currentControl->OnItemControlLost();
	m_pendingItemControlRequests.back()->OnItemControlGranted();
	return true;
}

bool UItemControlComponent::ReleaseItemControl(IItemControlRequester* requester)
{
	const IItemControlRequester* currentControl = m_pendingItemControlRequests.back();
	const auto it = std::find(m_pendingItemControlRequests.begin(), m_pendingItemControlRequests.end(), requester);
	if (it == m_pendingItemControlRequests.end())
		return false;
	m_pendingItemControlRequests.erase(it);
	
	if (currentControl != requester)
		return false;
	
	requester->OnItemControlLost();
	m_pendingItemControlRequests.back()->OnItemControlGranted();
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

