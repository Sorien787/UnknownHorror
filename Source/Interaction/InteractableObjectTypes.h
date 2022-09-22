// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"


UENUM()
enum InteractableObjectType
{
	DefaultPickup,
	LockerUse,
	Door,
	Wheel
};

UENUM()
enum InteractableObjectAnims
{
	Enter UMETA(DisplayName = "Enter"),
	Exit UMETA(DisplayName = "Exit"),
	Use UMETA(DisplayName = "Use")
};

class DEEPSEAHORROR_API InteractableObjectTypes
{

};
