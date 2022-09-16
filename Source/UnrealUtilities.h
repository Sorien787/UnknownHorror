// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class DEEPSEAHORROR_API UnrealUtilities
{
public:
	static FVector RaycastActorToWorldPosition(const UWorld* world, const float range, const AActor* pIgnoreActor);
	static TArray<FHitResult> RaycastActorToWorldHit(UWorld* world, const float range, const AActor* pIgnoreActor);
	static FRotator GetRotationMatrixToPlayer(const UWorld* world, const FVector objectLocation);
	static FTransform GetPlayerCameraTransform(const UWorld* world);
	UnrealUtilities();
	~UnrealUtilities();
};


template <class T>
class Grid
{
protected:
	std::vector<T> m_BasicGrid;

	FIntVector m_GridSize;

	FVector m_GridOrigin;

	float m_GridElementSize;

	int ConvertGridInputToIndex(FIntVector input) const
	{
		return input.Z * m_GridSize.X * m_GridSize.Y + input.Y * m_GridSize.X + input.X;
	}

	FIntVector ConvertFloatInputToGridInput(FVector floatInput) const
	{
		const FVector pos_gridSpace = (floatInput - m_GridOrigin) / m_GridElementSize;
		const FIntVector gridPosition =  FIntVector(FMath::Floor(pos_gridSpace.X), FMath::Floor(pos_gridSpace.Y), FMath::Floor(pos_gridSpace.Z));
		return gridPosition;
	}

	bool IsInputWithinGrid(FIntVector input) const
	{
		return input.X >= 0 && input.X < m_GridSize.X && input.Y >= 0 && input.Y < m_GridSize.X && input.Z >= 0 && input.Z < m_GridSize.X;
	}

	void DebugDrawGrid(const UWorld* pRenderWorld) const
	{
		DrawDebugBox(pRenderWorld, m_GridOrigin + FVector(m_GridSize) * m_GridElementSize / 2.0f, FVector(m_GridSize) * m_GridElementSize, FColor::Blue);
		const FVector gridSize = FVector(m_GridElementSize) / 2;
		for (int z = m_GridSize.Z-1; z >= 0; --z)
		{
			for (int y = m_GridSize.Y-1; y >= 0; --y)
			{
				for (int x = m_GridSize.X-1; x >= 0; --x)
				{
					const FIntVector gridPosition = FIntVector(x,y,z);
					const FVector gridPositionWorld = m_GridOrigin + FVector(gridPosition) * m_GridElementSize + gridSize;
					const int gridIndex = ConvertGridInputToIndex(gridPosition);
					const T gridValue = m_BasicGrid[gridIndex];
					const FColor gridColor = ConvertGridValueToColor(gridValue);
					DrawDebugBox(pRenderWorld, gridPositionWorld, gridSize, gridColor);
				}
			}
		}	
	}
	
	virtual FColor ConvertGridValueToColor(T& gridObject) const = 0;
public:
	virtual T SampleGrid(const float x, const float y, const float z) const
	{
		const FVector pos = FVector(x, y, z);
		const FIntVector pos_gridSpace = ConvertFloatInputToGridInput(pos);
		const int index = ConvertGridInputToIndex(pos_gridSpace);
		return m_BasicGrid[index];
	}
	
	bool IsWorldInputWithinGrid(FVector input) const
	{
		return IsInputWithinGrid(ConvertFloatInputToGridInput(input));
	}
	
	Grid(const int gridSizeX, const int gridSizeY, const int gridSizeZ, const float gridOriginX, const float gridOriginY, const float gridOriginZ, const float gridElemSize)
	: m_GridSize(gridSizeX, gridSizeY, gridSizeZ)
	, m_GridOrigin(gridOriginX, gridOriginY, gridOriginZ)
	, m_GridElementSize(gridElemSize)
	{
		m_BasicGrid.resize(gridSizeX * gridSizeY * gridSizeZ);
	}

	virtual ~Grid(){};
};