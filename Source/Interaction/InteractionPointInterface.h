// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractionPointInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UInteractionPointInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class DEEPSEAHORROR_API IInteractionPointInterface
{
	GENERATED_BODY()
	
	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintImplementableEvent)
	void InteractionWidgetReveal();

	UFUNCTION(BlueprintImplementableEvent)
		void InteractionWidgetHide();

	UFUNCTION(BlueprintImplementableEvent)
		void InteractionWidgetFocus();

	UFUNCTION(BlueprintImplementableEvent)
		void InteractionWidgetUnfocus();

	UFUNCTION(BlueprintImplementableEvent)
		void InteractionWidgetInteractFast();

	UFUNCTION(BlueprintImplementableEvent)
		void InteractionWidgetInteractSlow();
};
