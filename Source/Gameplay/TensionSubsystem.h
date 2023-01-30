// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interaction/InteractableInterface.h"
#include "Subsystems/WorldSubsystem.h"
#include "TensionSubsystem.generated.h"

class ALurkLocationActor;
/**
 * 
 */
UCLASS()
class DEEPSEAHORROR_API UTensionSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

	virtual TStatId GetStatId() const override;

	virtual void Tick(float DeltaTime) override;
	
public:
	
	UFUNCTION(BlueprintCallable)
	void AddSingleTensionInstance(float addTension);

	UFUNCTION(BlueprintCallable)
	float GetCurrentTension() const;

	UFUNCTION()
	void RegisterLurkLocation(const TScriptInterface<IInteractableInterface>& pLurkActor);

	UFUNCTION()
	void RegisterEscapeLocation(const TScriptInterface<IInteractableInterface>& pEscapeActor);
	
	UFUNCTION(BlueprintCallable)
	TScriptInterface<IInteractableInterface> GetLurkLocation(InteractionUserType type, FVector location) const;

	UFUNCTION(BlueprintCallable)
	TScriptInterface<IInteractableInterface> GetEscapeLocation(InteractionUserType type, FVector location) const;
	
	UFUNCTION(BlueprintCallable)
	void OnEntityEscaped(AActor* entity);
	
	UPROPERTY(EditAnywhere)
	float m_fMaximumTension;

	UPROPERTY(EditAnywhere)
	FRuntimeFloatCurve m_TensionDecayCurve;

	UPROPERTY(EditAnywhere)
	FVector m_EntityLocationCache;
	
	float m_fCurrentTension{0.0f};

	std::vector<IInteractableInterface*> m_LurkLocations;

	std::vector<IInteractableInterface*> m_EscapeLocations;
};
