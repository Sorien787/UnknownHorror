// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/BoxComponent.h"
#include "InteractionTriggerInterface.h"
#include "InteractableObjectBase.h"
#include "InteractionUserComponent.generated.h"


class AInteractableObject;


UENUM(BlueprintType)
enum class InteractionUserType : uint8
{
	Undefined UMETA(DisplayName = "Undefined", Hidden),
	PlayerSneak UMETA(DisplayName = "Player Sneak"),
	Player UMETA(DisplayName = "Player"),
	PlayerSprint UMETA(DisplayName = "Sprinting Player"),
	Schism UMETA(DisplayName = "Schism")

};

UINTERFACE(MinimalAPI, NotBlueprintable)
class UInteractingCharacter : public UInterface
{
	GENERATED_BODY()
};

class DEEPSEAHORROR_API IInteractingCharacter
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	virtual void IsInteractionAvailableOverride(const int interactorId, bool& returnResult) = 0;
	
	UFUNCTION(BlueprintCallable)
	virtual void GetPossibleAvailableInteractions(TArray<int>& result) = 0;
	
	UFUNCTION(BlueprintCallable)
	virtual void OnInteractWithInteractor(const int interactorId, bool& returnResult) = 0;

	UFUNCTION(BlueprintCallable)
	virtual FTransform GetDesiredTransformForInteraction(const int interactorId) = 0;

	UFUNCTION(BlueprintCallable)
	virtual void TryStopInteracting() = 0;
};

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

	bool IsCurrentlyInInteractionAnimation() const;

	float GetAnimCameraYaw() const;

	float GetAnimCameraPitch() const;
	
	void OnInteractionFinished();

	void OnInteractWithFocusedInteractable();

	void DisableInteractions();

	void EnableInteractions();

	void SetInteractionUserType(InteractionUserType userType);
	
	IInteractionTriggerInterface* ClosestInteractionQuery(bool ignoreCurrentInteractable = false) const;

	void AddInteractionEnterShape(UShapeComponent* pBox);

	void AddInteractionExitShape(UShapeComponent* pBox);

	const float GetInteractionRange() const;
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

public:
	UFUNCTION()
		void OnInteractButtonPressed();

	UPROPERTY(EditAnywhere)
		bool m_bShouldTriggerWidgets{ true };

	UPROPERTY(EditAnywhere)
		bool m_bNeedsRaycastToDetectInteractionProximity{true};

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		bool m_bIsPlayerInteractionUser{true};
	
	UPROPERTY(EditAnywhere)
		float m_fInteractionRange{3000.0f};
	
	UPROPERTY(EditAnywhere)
		UShapeComponent* m_pEnterShape;

	UPROPERTY(EditAnywhere)
		UShapeComponent* m_pExitShape;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		InteractionUserType m_UserType;
	
	UPROPERTY(EditAnywhere)
		FVector m_DefaultHandOffset;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float m_DefaultCameraYawTolerance = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float m_DefaultCameraPitchTolerance = 20.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		bool m_bIsCurrentlyInInteractionAnimation = false;
};
