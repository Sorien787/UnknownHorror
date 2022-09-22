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

	void AddInteractionEnterBox(UBoxComponent* pBox);

	void AddInteractionExitBox(UBoxComponent* pBox);

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

	UPROPERTY(EditAnywhere)
		bool m_bIsPlayerInteractionUser{true};
	
	UPROPERTY(EditAnywhere)
		float m_fInteractionRange{3000.0f};
	
	UPROPERTY(EditAnywhere)
		UBoxComponent* m_pEnterBox;

	UPROPERTY(EditAnywhere)
		UBoxComponent* m_pExitBox;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		InteractionUserType m_UserType;
	
	UPROPERTY(EditAnywhere)
		FVector m_DefaultHandOffset;

	UPROPERTY(EditAnywhere)
		float m_DefaultCameraYawTolerance = 20.0f;

	UPROPERTY(EditAnywhere)
		float m_DefaultCameraPitchTolerance = 20.0f;

	bool m_bIsCurrentlyInInteractionAnimation = false;
};
