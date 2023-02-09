// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemControl/ItemControlComponent.h"
#include "UObject/Interface.h"
#include "InteractionComponentInterface.generated.h"

class IInteractableInterface;

UENUM(BlueprintType)
enum class InteractionUserType : uint8
{
	Undefined UMETA(DisplayName = "Undefined", Hidden),
	PlayerSneak UMETA(DisplayName = "Player Sneak"),
	Player UMETA(DisplayName = "Player"),
	PlayerSprint UMETA(DisplayName = "Sprinting Player"),
	Schism UMETA(DisplayName = "Schism")

};

// This class does not need to be modified.
UINTERFACE(MinimalAPI, NotBlueprintable)
class UInteractionComponentInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class DEEPSEAHORROR_API IInteractionComponentInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintCallable)
	virtual void OnInteractionStarted(const TScriptInterface<IInteractableInterface> interf) = 0;

	UFUNCTION(BlueprintCallable)
	virtual void OnInteractionFinished(const TScriptInterface<IInteractableInterface> interf)= 0;

	UFUNCTION(BlueprintCallable)
	virtual bool IsInteracting() const = 0;

	UFUNCTION(BlueprintCallable)
	virtual bool TryCancelCurrentInteraction_Implementation() = 0;
	
	UFUNCTION(BlueprintCallable)
	virtual AActor* GetInteractionOwner() const = 0;
	
	UFUNCTION(BlueprintCallable)
	virtual InteractionUserType GetInteractionUserType() const = 0;

	UFUNCTION(BlueprintCallable)
	virtual UObject* GetComponentObject() = 0;
};

UCLASS(ClassGroup=(Custom), NotBlueprintable)
class DEEPSEAHORROR_API UInteractionComponentBase: public UActorComponent, public IInteractionComponentInterface, public IItemControlRequester
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	virtual void OnInteractionStarted(const TScriptInterface<IInteractableInterface> interf) override;

	UFUNCTION(BlueprintCallable)
	virtual void OnInteractionFinished(const TScriptInterface<IInteractableInterface> interf) override;

	UFUNCTION(BlueprintCallable)
	virtual bool IsInteracting() const override {return m_bIsInteracting;}

	UFUNCTION(BlueprintCallable)
	virtual bool TryCancelCurrentInteraction_Implementation() override;
	
	UFUNCTION(BlueprintCallable)
	virtual AActor* GetInteractionOwner() const override {return GetOwner();}
	
	UFUNCTION(BlueprintCallable)
	virtual InteractionUserType GetInteractionUserType() const override {return m_UserType;}

	UFUNCTION(BlueprintCallable)
	virtual UObject* GetComponentObject() override {return this;}

	virtual void OnItemControlGranted_Implementation(AActor* pActor) override{}
	
	virtual void OnItemControlLost_Implementation(AActor* pActor) override{}
	
	virtual int GetPriority() const override;

	UPROPERTY(EditAnywhere)
	InteractionUserType m_UserType;

	UPROPERTY(BlueprintReadOnly)
	bool m_bIsInteracting = false;

	UPROPERTY(EditAnywhere)
	int m_ItemControlPriority = 0;
	
	UPROPERTY(BlueprintReadOnly)
	TScriptInterface<IInteractableInterface> m_pCurrentlyInteractingInterface;
};