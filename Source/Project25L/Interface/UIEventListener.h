// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "UIEventListener.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UUIEventListener : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PROJECT25L_API IUIEventListener
{
	GENERATED_BODY()

public:
	virtual void BindEvents() PURE_VIRTUAL(IUIEventListener::BindEvents;);
};
