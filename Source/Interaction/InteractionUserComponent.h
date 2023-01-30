// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractionTriggerInterface.h"
#include "InteractableObjectBase.h"
#include "InteractionComponentInterface.h"
#include "InteractionUserComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable)
class DEEPSEAHORROR_API UInteractionPlayerComponent : public UInteractionComponentBase
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInteractionPlayerComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// UActorComponent
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	// ~UActorComponent

	// UInteractionComponentBase
	virtual void OnInteractionStarted(const TScriptInterface<IInteractableInterface> interf) override;

	virtual void OnInteractionFinished(const TScriptInterface<IInteractableInterface> interf) override;
	// ~UInteractionComponentBase
	
	float GetAnimCameraYaw() const;

	float GetAnimCameraPitch() const;
	
	const float GetInteractionRange() const;

	void AddInteractionEnterShape(UShapeComponent* pBox);

	void AddInteractionExitShape(UShapeComponent* pBox);

	void SetInteractionUserType(InteractionUserType userType);
	
private:

	UFUNCTION()
		void OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
		void OnBoxEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	bool m_bInteractionsEnabled{ true };

	TSet<IInteractionTriggerInterface*> m_InteractionCandidates;
	IInteractionTriggerInterface* m_pCurrentFocusedInteractionPoint = nullptr;
	IInteractionTriggerInterface* m_pCurrentUsingInteractionPoint = nullptr;

	void ClearFocusedInteractable();
	
	void FocusedInteractionUpdate();

	void RevealInteractionUpdate();
	
	void SetNewFocusedInteractable(IInteractionTriggerInterface* pNewInteractable);
	
	void OnInteractWithFocusedInteractable();

	void DisableInteractions();

	void EnableInteractions();

	IInteractionTriggerInterface* ClosestInteractionQuery(bool ignoreCurrentInteractable = false);

public:
	UFUNCTION()
		void OnInteractButtonPressed();
	
	UPROPERTY(EditAnywhere)
		float m_fInteractionRange{3000.0f};
	
	UPROPERTY(EditAnywhere)
		UShapeComponent* m_pEnterShape;

	UPROPERTY(EditAnywhere)
		UShapeComponent* m_pExitShape;

	UPROPERTY(EditAnywhere)
		FVector m_DefaultHandOffset;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float m_DefaultCameraYawTolerance = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float m_DefaultCameraPitchTolerance = 20.0f;
};
