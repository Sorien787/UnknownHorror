// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../Common/UnrealUtilities.h"
#include "CoreMinimal.h"
#include "../Common/ListenerUtils.h"

/**
 * 
 */

class DEEPSEAHORROR_API HazeListener
{
public:
	virtual void OnRefreshHazeGridPosition() {}
};

class DEEPSEAHORROR_API HazeComponentListener
{
public:
	virtual void OnHazeStart() {};

	virtual void OnHazeFinish() {};

	virtual void OnHazeSetValue(float value) {};

	virtual void OnHazeStrengthChanged(float value) {};
};



class DiffusionGrid : public Grid<float>
{
protected:
	virtual FColor ConvertGridValueToColor(const float& gridObject) const override;
private:
	float m_DiffusionCoefficient;

	float m_DrainageCoefficient;
	
	float m_FloatValForMaxColor;

	static const std::array<FIntVector, 6> m_DirectionalArray;

	
public:
	DiffusionGrid(const float floatValForMaxColor, const float gridSizeX, const float gridSizeY, const float gridSizeZ, const float gridOriginX, const float gridOriginY, const float gridOriginZ, const float gridElemSize, const float diffusionCoefficient, const float drainageCoefficient)
	: Grid(gridSizeX, gridSizeY, gridSizeZ, gridOriginX, gridOriginY, gridOriginZ, gridElemSize)
	, m_DiffusionCoefficient{diffusionCoefficient}
	, m_DrainageCoefficient{drainageCoefficient}
	, m_FloatValForMaxColor{floatValForMaxColor}
	{}
	
	DiffusionGrid() : Grid(0, 0, 0, 0, 0, 0, 0)
	, m_DiffusionCoefficient{0}
	, m_DrainageCoefficient{0}
	, m_FloatValForMaxColor{0}
	{};
	
	~DiffusionGrid() override;

	DiffusionGrid& operator=(const DiffusionGrid& other)
	{
		m_DiffusionCoefficient = other.m_DiffusionCoefficient;
		m_DrainageCoefficient = other.m_DrainageCoefficient;
		m_FloatValForMaxColor = other.m_FloatValForMaxColor;
		m_BasicGrid = other.m_BasicGrid;
		m_GridSize = other.m_GridSize;
		m_GridOrigin = other.m_GridOrigin;
		m_GridElementSize = other.m_GridElementSize;
		return *this;
	}
	
	void RunDiffusionCycle(const float deltaTime);

	virtual float SampleGrid(const float x, const float y, const float z) const override;

	void AddDiffusionSource(const float x, const float y, const float z, const float sourceStrength);
};