// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ListenerUtils.h"

/**
 * 
 */

class DEEPSEAHORROR_API HazeListener
{
public:
	virtual void OnRefreshHazeStrength() {}
};

class DEEPSEAHORROR_API HazeComponentListener
{
public:
	virtual void OnHazeStart() {};

	virtual void OnHazeFinish() {};

	virtual void OnHazeSetValue(float value) {};
};