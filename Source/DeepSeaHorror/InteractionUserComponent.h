// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/BoxComponent.h"
#include "InteractableInterface.h"
#include "InteractionUserComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DEEPSEAHORROR_API UInteractionUserComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInteractionUserComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void OnDisengageWithInteraction();

	void OnInteractWithFocusedInteractable();

	void AddInteractionEnterBox(UBoxComponent* pBox);

	void AddInteractionExitBox(UBoxComponent* pBox);
private:

	UFUNCTION()
		void OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
		void OnBoxEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	TSet<IInteractableInterface*> m_InteractionCandidates;

	bool m_InteractionUse{ false };
	bool m_bInteractionsEnabled{ false };
	bool m_bCurrentFocusedInteractableInUse{ false };
	IInteractableInterface* m_pCurrentFocusedInteractable = nullptr;

	void ClearCurrentInteractable();
	FVector TryRaycastToNearestPosition();
	bool IsActorInView(AActor* pActor) const;
	void SetNewInteractable(IInteractableInterface* newInteractable);

public:
	UFUNCTION()
		void OnInteractButtonPressed();

	UPROPERTY(EditAnywhere)
		bool m_bShouldTriggerWidgets{ true };

	UPROPERTY(EditAnywhere)
		bool m_bNeedsRaycastToDetectInteractionProximity{true};

	UPROPERTY(EditAnywhere)
		bool m_bIsPlayerInteractionUser{true};

	UPROPERTY(EditAnywhere)
		float m_fInteractionRange{4.0f};
	
	UPROPERTY(EditAnywhere)
		UBoxComponent* m_pEnterBox;

	UPROPERTY(EditAnywhere)
		UBoxComponent* m_pExitBox;
};
