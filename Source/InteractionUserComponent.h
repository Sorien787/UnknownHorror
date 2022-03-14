// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/BoxComponent.h"
#include "InteractableInterface.h"
#include "InteractionPoint.h"
#include "InteractableObjectBase.h"
#include "InteractableObjectTypes.h"
#include "InteractionUserComponent.generated.h"


class AInteractableObject;

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
	
	void OnInteractionFinished();

	void OnInteractWithFocusedInteractable();

	void DisableInteractions();

	void EnableInteractions();
	
	AInteractionPoint* ClosestInteractionQuery(bool ignoreCurrentInteractable = false) const;

	void AddInteractionEnterBox(UBoxComponent* pBox);

	void AddInteractionExitBox(UBoxComponent* pBox);

	bool IsHandFull() const;

	bool AddObjectToHand(AInteractableObject* pInteractableObject);

	AInteractableObject* RemoveCurrentObjectInHand();
private:

	UFUNCTION()
		void OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
		void OnBoxEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	bool m_bInteractionsEnabled{ true };
	
	TSet<AInteractionPoint*> m_InteractionCandidates;
	AInteractionPoint* m_pCurrentFocusedInteractionPoint = nullptr;
	AInteractionPoint* m_pCurrentUsingInteractionPoint = nullptr;

	void ClearFocusedInteractable();
	
	void FocusedInteractionUpdate();

	void RevealInteractionUpdate();
	
	void SetNewFocusedInteractable(AInteractionPoint* pNewInteractable)

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
		float m_fInteractionRange{3000.0f};
	
	UPROPERTY(EditAnywhere)
		UBoxComponent* m_pEnterBox;

	UPROPERTY(EditAnywhere)
		UBoxComponent* m_pExitBox;
};
