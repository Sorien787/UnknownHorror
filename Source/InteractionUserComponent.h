// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/BoxComponent.h"
#include "InteractableInterface.h"
#include "InteractableObjectBase.h"
#include "InteractableObjectTypes.h"
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

	float GetDropRange() const;
	
	void TriggerInteraction();
	
	AInteractableObjectBase* ClosestInteractionQuery(bool ignoreCurrentInteractable = false) const;
	
	bool HasInteractionAnim(InteractableObjectType type) const;

	void AddInteractionEnterBox(UBoxComponent* pBox);

	void AddInteractionExitBox(UBoxComponent* pBox);
private:

	UFUNCTION()
		void OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
		void OnBoxEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	TSet<AInteractableObjectBase*> m_InteractionCandidates;
	
	bool m_bInteractionsEnabled{ false };
	
	AInteractableObjectBase* m_pCurrentFocusedInteractable = nullptr;
	AInteractableObjectBase* m_pCurrentUsingInteractable = nullptr;

	void ClearFocusedInteractable();
	
	void FocusedInteractionUpdate();
	
	void SetNewFocusedInteractable(AInteractableObjectBase* pNewInteractable);

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
		float m_DropRange{3000.0f};
	
	UPROPERTY(EditAnywhere)
		float m_fInteractionRange{3000.0f};
	
	UPROPERTY(EditAnywhere)
		UBoxComponent* m_pEnterBox;

	UPROPERTY(EditAnywhere)
		UBoxComponent* m_pExitBox;
};
