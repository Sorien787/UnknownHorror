// Fill out your copyright notice in the Description page of Project Settings.

#include "UnrealUtilities.h"

UnrealUtilities::UnrealUtilities()
{
}

UnrealUtilities::~UnrealUtilities()
{
}

bool UnrealUtilities::IsInFrustrum( FVector location, float size, UWorld* pWorld)
{
	ULocalPlayer* LocalPlayer = pWorld->GetFirstLocalPlayerFromController();
	
	if (LocalPlayer == nullptr || LocalPlayer->ViewportClient == nullptr || !LocalPlayer->ViewportClient->Viewport)
		return false;
	
	FSceneViewFamilyContext ViewFamily(FSceneViewFamily::ConstructionValues(
		LocalPlayer->ViewportClient->Viewport,
		pWorld->Scene,
		LocalPlayer->ViewportClient->EngineShowFlags)
		.SetRealtimeUpdate(true));

	FVector ViewLocation;
	FRotator ViewRotation;
	FSceneView* SceneView = LocalPlayer->CalcSceneView(&ViewFamily, ViewLocation, ViewRotation, LocalPlayer->ViewportClient->Viewport);
	
	if (SceneView == nullptr)
		return false;
	
	return SceneView->ViewFrustum.IntersectSphere(location, size);
}

float UnrealUtilities::GetRadAngleBetweenVectors(FVector a, FVector b)
{
	return FMath::Acos(FVector::DotProduct(a.GetUnsafeNormal(), b.GetUnsafeNormal()));
}

FVector UnrealUtilities::GetArbitraryNormalVector(FVector basisA)
{
	FVector someDirection = FVector(1, 0, 0);
	
	if(basisA.Dot(someDirection) > 1 - FLT_EPSILON)
		someDirection = FVector(0, 1, 0);

	FVector basisB = someDirection - someDirection.Dot(basisA) * basisA;
	basisB.Normalize();
	
	return basisB;
}

void UnrealUtilities::DrawLineTrace(UWorld* world, bool hit, FVector start, FVector end, FHitResult hitRes)
{
	DrawDebugLine(world, start, hit ? hitRes.Location : end, hit ? FColor::Green : FColor::Red, false, 0.0f, 0, 5.0f);
}

FVector UnrealUtilities::RaycastActorToWorldPosition(const UWorld* world, const float range, const AActor* pIgnoreActor)
{
	FVector playerViewPointLocation;
	FRotator playerRotationInformation;
	
	world->GetFirstPlayerController()->GetPlayerViewPoint(OUT playerViewPointLocation, OUT playerRotationInformation);
	
	FVector lineTraceEnd = playerViewPointLocation + playerRotationInformation.Vector() * range;

	FHitResult hit;
	// false to ignore complex collisions
	FCollisionQueryParams traceParams(FName(TEXT("Raycast To Interactable")), true, pIgnoreActor);

	world->LineTraceSingleByObjectType(
		OUT hit,
		playerViewPointLocation,
		lineTraceEnd,
		FCollisionObjectQueryParams(ECollisionChannel::ECC_Visibility),
		traceParams);

	AActor* pActorHit = hit.GetActor();
	
	if (!pActorHit)
		return lineTraceEnd;
	
	return  hit.Location;
}

TArray<FHitResult> UnrealUtilities::RaycastActorToWorldHit(UWorld* world, const float range, const AActor* pIgnoreActor)
{
	FVector playerViewPointLocation;
	FRotator playerRotationInformation;
	
	world->GetFirstPlayerController()->GetPlayerViewPoint(OUT playerViewPointLocation, OUT playerRotationInformation);
	
	FVector lineTraceEnd = playerViewPointLocation + playerRotationInformation.Vector() * range;

	FHitResult hit;
	// false to ignore complex collisions

	static FName TraceTag = TEXT("TraceTag");
	FCollisionQueryParams traceParams(TraceTag, false, pIgnoreActor);
	TArray<FHitResult> outHits;
	world->LineTraceMultiByObjectType(
		OUT outHits,
		playerViewPointLocation,
		lineTraceEnd,
		FCollisionObjectQueryParams(ECollisionChannel::ECC_WorldStatic | ECollisionChannel::ECC_WorldDynamic),
		traceParams);
	
	return outHits;
}

FRotator UnrealUtilities::GetRotationMatrixToPlayer(const UWorld* world, const FVector objectLocation)
{
	FVector cameraLocation = world->GetFirstPlayerController()->PlayerCameraManager->GetCameraLocation();

	return (cameraLocation - objectLocation).Rotation();
}

FTransform UnrealUtilities::GetPlayerCameraTransform(const UWorld* world)
{
	return world->GetFirstPlayerController()->PlayerCameraManager->GetTransform();
}

FVector UnrealUtilities::VectorSpringInterpCD(FVector Current, FVector Target, FVector& Velocity, float DeltaTime, float InterpSpeed, float MaxVelocity)
{
	const FVector n1 = Velocity - (Current - Target) * (InterpSpeed * InterpSpeed * DeltaTime);
	const float n2 = 1.f + InterpSpeed * DeltaTime;
	if (MaxVelocity > 0.f)
	{
		Velocity = (n1 / (n2 * n2)).GetClampedToMaxSize(MaxVelocity);
	}
	else
	{
		Velocity = n1 / (n2 * n2);
	}
	return Current + Velocity * DeltaTime;
}

float UnrealUtilities::FloatSpringInterpCD(float Current, float Target, float& Velocity, float DeltaTime, float InterpSpeed, float MaxVelocity)
{
	const float n1 = Velocity - (Current - Target) * (InterpSpeed * InterpSpeed * DeltaTime);
	const float n2 = 1.f + InterpSpeed * DeltaTime;

	Velocity = (MaxVelocity > 0.f) ? FMath::Min(n1 / (n2 * n2), MaxVelocity) : n1 / (n2 * n2);

	return Current + Velocity * DeltaTime;
}

FRotator UnrealUtilities::RotatorSpringInterpCD(FRotator Current, FRotator Target, FVector4& Velocity, float DeltaTime, float InterpSpeed, float MaxVelocity)
{
	return QuatSpringInterpCD(Current.Quaternion(), Target.Quaternion(), Velocity, DeltaTime, InterpSpeed, MaxVelocity).Rotator();
}

FORCEINLINE FVector4 UnrealUtilities::QuatToVector4(const FQuat& Quat)
{
	return FVector4(Quat.X, Quat.Y, Quat.Z, Quat.W);
}

FORCEINLINE FVector4 UnrealUtilities::ClampVector4(FVector4 Target, float MaxSize)
{
	if (MaxSize < KINDA_SMALL_NUMBER)
	{
		return FVector4(0.f, 0.f, 0.f, 0.f);
	}

	const float VSq = Target.SizeSquared();
	if (VSq > FMath::Square(MaxSize))
	{
		const float Scale = MaxSize * FMath::InvSqrt(VSq);
		return FVector4(Target.X*Scale, Target.Y*Scale, Target.Z*Scale, Target.W*Scale);
	}
	else
	{
		return Target;
	}
}

FQuat UnrealUtilities::QuatSpringInterpCD(FQuat Current, FQuat Target, FVector4& Velocity, float DeltaTime, float InterpSpeed, float MaxVelocity)
{
	// Here would it be better to make operations directly on FQuat? 
	// I can't find FQuat operators code to check, so I prefer those conversions...
	FVector4 currentVector = QuatToVector4(Current);
	FVector4 targetVector = QuatToVector4(Target);

	// We can use either of vtarget/-vtarget. Use closer one. 
	// If using FQuat, might FQuat::Squad() be usesul here?
	if (Dot4(currentVector, targetVector) < 0.f) targetVector = -targetVector;

	const FVector4 n1 = Velocity - (currentVector - targetVector) * (InterpSpeed * InterpSpeed * DeltaTime);
	const float n2 = 1.f + InterpSpeed * DeltaTime;

	if (MaxVelocity > 0.f)
	{
		Velocity = ClampVector4(n1 / (n2 * n2), MaxVelocity);
	}
	else
	{
		Velocity = n1 / (n2 * n2);
	}
	// Apply delta on current
	currentVector = (currentVector + Velocity * DeltaTime);

	// Normalizing gave odd results, it looks fine this way but don't ask me why...
	return FQuat(currentVector.X, currentVector.Y, currentVector.Z, currentVector.W);
}


