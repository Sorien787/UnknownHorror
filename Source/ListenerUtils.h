// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <vector>

#include "CoreMinimal.h"

/**
 * 
 */

template<class T>
class DEEPSEAHORROR_API ListenerUtils
{
struct SListenerImpl
{
 SListenerImpl(T* pL, const char* name) : pListener(pL), listenerName(std::move(name)){}

 inline bool operator==(const SListenerImpl& other) const{return pListener == other.pListener;}
 inline bool operator==(const T* other) const{return pListener == other;}
 T* pListener;
 const char* listenerName;
};

 typedef std::vector<SListenerImpl> TListeners; 
 TListeners m_Listeners;
 ListenerUtils() = default;
 ListenerUtils(const ListenerUtils&) = delete;
 ListenerUtils(const ListenerUtils&&) = delete;
public:
 inline bool IsListener(T* pListener) const
 {
  return std::find_if(m_Listeners.begin(), m_Listeners.end(), [pListener](const SListenerImpl &listenerImpl) {return listenerImpl.pListener == pListener; });
 }

 inline void AddListener(T* pListener, const char* name)
 {
  m_Listeners.emplace_back(pListener, name);
 }

 inline void RemoveListener(T* pListener)
 {
  auto& it = std::find_if(m_Listeners.begin(), m_Listeners.end(), [pListener](const SListenerImpl &listenerImpl){return listenerImpl.pListener == pListener;});
  if (it != m_Listeners.end())
  {
   m_Listeners.erase(it);
  }
 }

 template<typename... UArgs, typename... Args>
 inline void Notify(void (T::*pfct)(UArgs...), Args&&... args)
 {
  for (size_t i = 0; i < m_Listeners.size(); i++)
  {
   const SListenerImpl& listenerImpl = m_Listeners[i];
   (*(listenerImpl).pListener.*pfct)(std::forward<Args>(args...));
  }
 }

};
