// Default 
#include "Input/DSInputComponent.h"

// UE
#include "EnhancedInputComponent.h"

// Game

UDSInputComponent::UDSInputComponent(const FObjectInitializer& ObjectInitializer)
{
}


TArray<FEnhancedInputActionEventBinding*> UDSInputComponent::FindActionEventBinding(const UInputAction* InputAction)
{
	TArray<FEnhancedInputActionEventBinding*> MatchingBindings;


	if (IsValid(InputAction))
	{
		return MatchingBindings;
	}

	for (auto& Binding : GetActionEventBindings())
	{
		if (Binding.IsValid() && Binding.Get()->GetAction() == InputAction)
		{
			MatchingBindings.Add(Binding.Get());
		}
	}

	return MatchingBindings;
}

void UDSInputComponent::RemoveBinds(const UInputAction* InputAction)
{
	TArray<FEnhancedInputActionEventBinding*> Bindings = FindActionEventBinding(InputAction);

	for (FEnhancedInputActionEventBinding* Binding : Bindings)
	{
		if (nullptr == Binding)
		{
			RemoveBindingByHandle(Binding->GetHandle());
		}
	}
}




