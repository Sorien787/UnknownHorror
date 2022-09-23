// Fill out your copyright notice in the Description page of Project Settings.

#include "HazeUtils.h"
#include "array"


DiffusionGrid::~DiffusionGrid()
{
	
}

const std::array<FIntVector, 6> DiffusionGrid::m_DirectionalArray	{
	FIntVector(0, 0, 1),
	FIntVector(0, 0, -1),
	FIntVector(0, 1, 0),
	FIntVector(0, -1, 0),
	FIntVector(1, 0, 0),
	FIntVector(-1, 0, 0)
};

void DiffusionGrid::RunDiffusionCycle(const float deltaTime)
{
	for (int z = m_GridSize.Z-1; z >= 0; --z)
	{
		for (int y = m_GridSize.Y-1; y >= 0; --y)
		{
			for (int x = m_GridSize.X-1; x >= 0; --x)
			{
				// its not really mathematically correct to do it like this
				// but let's just iterate through the grid and spread to adjacent grids
				const float distanceScale = m_GridElementSize / 1000.0f;
				const FIntVector gridPosition = FIntVector(x, y, z);
				const int gridIndex = ConvertGridInputToIndex(gridPosition);
				const float gridValue = m_BasicGrid[gridIndex] * ( 1 - deltaTime * m_DrainageCoefficient * FMath::Pow(distanceScale, 3));
				const float remainingProportion = 1.0f / (1.0f + deltaTime * m_DiffusionCoefficient * (1.0f / distanceScale));
				m_BasicGrid[gridIndex] = gridValue * remainingProportion;
				const float diffusedAmount = (gridValue * (1.0f - remainingProportion)) / 6.0f;

				for (int w = m_DirectionalArray.size()-1; w >= 0; --w)
				{
					FIntVector relevantDirection = m_DirectionalArray[w];
					const FIntVector diffusedGridElement = gridPosition + relevantDirection;

					// if we go outside the bounds of the grid, we just diffuse out of it and the data is lost
					// too bad!
					if (!IsInputWithinGrid(diffusedGridElement))
						continue;

					const int diffusedGridIndex = ConvertGridInputToIndex(diffusedGridElement);
					m_BasicGrid[diffusedGridIndex] += diffusedAmount;
				}
			}
		}
	}
}

float DiffusionGrid::SampleGrid(const float x, const float y, const float z) const
{
	FVector pos = FVector(x, y, z);
	const FVector pos_gridSpace = (pos - m_GridOrigin) / m_GridElementSize;
	const FIntVector initialGridPosition =  FIntVector(FMath::Floor(pos_gridSpace.X), FMath::Floor(pos_gridSpace.Y), FMath::Floor(pos_gridSpace.Z));

	// remove 1/2 to all sides: since we want to take samples as if they're at a distance from the *center*, but the indices are at the lowest corner
	// rather than adding 1/2 to each grid's position
	// this is effectively the same
	int numSamples = 0;
	float totLen = 0.0f;
	float totDensity = 0.0f;
	// don't just sample the grid where we are - figure out what the adjacent values are, and take the weighted average W.R.T the distance between their centers.
	for (int zPos = 1; zPos >= -1; --zPos)
	{
		for (int yPos = 1; yPos >= -1; --yPos)
		{
			for (int xPos = 1; xPos >= -1; --xPos)
			{
				const FIntVector gridPosition = FIntVector(initialGridPosition.X + xPos, initialGridPosition.Y + yPos, initialGridPosition.Z + zPos);
				if(!IsInputWithinGrid(gridPosition))
					continue;
				
				const FVector offset = FVector(gridPosition.X - pos_gridSpace.X - 0.5f, gridPosition.Y - pos_gridSpace.Y - 0.5f, gridPosition.Z - pos_gridSpace.Z - 0.5f);
				const float len = offset.Length();
				const float lenScale = FMath::Max(0.0f, 1.0f - len);
				totLen += lenScale;
				totDensity += lenScale * m_BasicGrid[ConvertGridInputToIndex(gridPosition)];
				numSamples++;
			}
		}
	}
	return totDensity / totLen;
}

void DiffusionGrid::AddDiffusionSource(const float x, const float y, const float z, const float sourceStrength)
{
	const FVector pos = FVector(x, y, z);
	const FIntVector gridPosition = ConvertFloatInputToGridInput(pos);
	if (!IsInputWithinGrid(gridPosition))
		return;
	const int index = ConvertGridInputToIndex(gridPosition);
	m_BasicGrid[index] += sourceStrength;
}

FColor DiffusionGrid::ConvertGridValueToColor(const float& gridObject) const
{
	const float lerpVal = FMath::Clamp(gridObject/m_FloatValForMaxColor, 0.0f, 1.0f);
	const float resultantColor = FMath::Lerp(0.0f, 255.0f, lerpVal);
	const float resultantAlpha = FMath::Lerp(0.0f, 1.0f, lerpVal);
	const uint8 cast = (uint8)FMath::Floor(resultantColor);
    return FColor(cast, 0.0f, 0.0f, resultantAlpha);
}
