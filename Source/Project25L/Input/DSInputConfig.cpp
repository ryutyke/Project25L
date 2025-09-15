//Default
#include "Input/DSInputConfig.h"

//UE

//Game


UDSInputConfig::UDSInputConfig(const FObjectInitializer& ObjectInitializer)
{
}

const UInputAction* UDSInputConfig::FindNativeInputActionForTag(const FGameplayTag& InputTag) const
{
	
	for (const FDSInputAction& Action : NativeInputActions)
	{
		if (Action.InputAction && (Action.InputTag == InputTag))
		{
			return Action.InputAction;
		}
	}

	return nullptr;
}

const UInputAction* UDSInputConfig::FindDualInputActionsForTag(const FGameplayTag& InputTag) const
{
	for (const FDSInputAction& Action : DualInputActions)
	{
		if (Action.InputAction && (Action.InputTag == InputTag))
		{
			return Action.InputAction;
		}
	}

	return nullptr;
}

const UInputAction* UDSInputConfig::FindTripleInputActionsForTag(const FGameplayTag& InputTag) const
{
	for (const FDSInputAction& Action : TripleInputActions)
	{
		if (Action.InputAction && (Action.InputTag == InputTag))
		{
			return Action.InputAction;
		}
	}

	return nullptr;
}