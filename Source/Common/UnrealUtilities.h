// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class DEEPSEAHORROR_API UnrealUtilities
{
public:
	template<class T>
	class RisingEdgeTrigger
	{
	public:
		RisingEdgeTrigger(T initVal){storedValue = initVal;}
		bool PollEdge(T newValue){ bool returnVal = false; if (newValue > storedValue){returnVal = true;} storedValue = newValue; return returnVal;}
	private:
		T storedValue;
	};

	template<class T>
	class GreaterThanTrigger
	{
	public:
		GreaterThanTrigger(T initVal){triggerValue = initVal;}
		bool PollEdge(T newValue){ bool returnVal = false; if (newValue > triggerValue){triggered = true; returnVal = true;}else{triggered = false;} return returnVal;}
	private:
		T triggerValue;
		bool triggered = false;
	};

	template<class T>
	class LessThanTrigger
	{
	public:
		LessThanTrigger(T initVal){triggerValue = initVal;}
		bool PollEdge(T newValue){ bool returnVal = false; if (newValue < triggerValue){triggered = true; returnVal = true;}else{triggered = false;} return returnVal;}
	private:
		T triggerValue;
		bool triggered = false;
	};

	template<class T>
	class FallingEdgeTrigger
	{
	public:
		FallingEdgeTrigger(T initVal){storedValue = initVal;}
		bool PollEdge(T newValue){ bool returnVal = false; if (newValue < storedValue){returnVal = true;}storedValue = newValue; return returnVal;}
	private:
		T storedValue;
	};
	
	static FVector RaycastActorToWorldPosition(const UWorld* world, const float range, const AActor* pIgnoreActor);
	static TArray<FHitResult> RaycastActorToWorldHit(UWorld* world, const float range, const AActor* pIgnoreActor);
	static FRotator GetRotationMatrixToPlayer(const UWorld* world, const FVector objectLocation);
	static FTransform GetPlayerCameraTransform(const UWorld* world);
	FVector VectorSpringInterpCD(FVector Current, FVector Target, FVector& Velocity, float DeltaTime, float InterpSpeed,
	                             float MaxVelocity);
	static bool IsInFrustrum( FVector location, float size, UWorld* pWorld);
	static float GetRadAngleBetweenVectors(FVector a, FVector b);
	static FVector GetArbitraryNormalVector(FVector basisA);
	static void DrawLineTrace(UWorld* world, bool hit, FVector start, FVector end, FHitResult hitRes);
	template<class C>
	static C* GetComponentFromActor(AActor* pOwner);

	static float FloatSpringInterpCD(float Current, float Target, float& Velocity, float DeltaTime, float InterpSpeed, float MaxVelocity);
	static FRotator RotatorSpringInterpCD(FRotator Current, FRotator Target, FVector4& Velocity, float DeltaTime, float InterpSpeed, float MaxVelocity);
	static FVector4 QuatToVector4(const FQuat& Quat);
	static FVector4 ClampVector4(FVector4 Target, float MaxSize);
	static FQuat QuatSpringInterpCD(FQuat Current, FQuat Target, FVector4& Velocity, float DeltaTime, float InterpSpeed, float MaxVelocity);
	UnrealUtilities();
	~UnrealUtilities();
};

template <class C>
C* UnrealUtilities::GetComponentFromActor(AActor* pOwner)
{
	TArray<C*> components;
	pOwner->GetComponents<C>(components);
	if (components.Num() == 0)
		return nullptr;
	return components[0];
}


template <class T>
class Grid
{
protected:
	std::vector<T> m_BasicGrid;

	FIntVector m_GridSize;

	FVector m_GridOrigin;

	T m_defaultValue;

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

	virtual FColor ConvertGridValueToColor(const T& gridObject) const = 0;
	virtual bool ShouldDebugDrawGridPoint(const T& gridObject) const = 0;
public:
	virtual T SampleGrid(const float x, const float y, const float z) const
	{
		const FVector pos = FVector(x, y, z);
		const FIntVector pos_gridSpace = ConvertFloatInputToGridInput(pos);
		const int index = ConvertGridInputToIndex(pos_gridSpace);
		
		if (index < 0 || index >= m_BasicGrid.size())
			return m_defaultValue;
		
		return m_BasicGrid[index];
	}
	
	void DebugDrawGrid(const UWorld* pRenderWorld) const
	{
		DrawDebugBox(pRenderWorld, m_GridOrigin + FVector(m_GridSize) * m_GridElementSize / 2.0f, FVector(m_GridSize) * m_GridElementSize / 2, FColor::Blue);
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
					if (!ShouldDebugDrawGridPoint(gridValue))
						continue;
					const FColor gridColor = ConvertGridValueToColor(gridValue);
					DrawDebugBox(pRenderWorld, gridPositionWorld, gridSize, gridColor);
				}
			}
		}	
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
