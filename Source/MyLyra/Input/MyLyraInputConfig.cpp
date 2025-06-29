// Fill out your copyright notice in the Description page of Project Settings.

#include "MyLyraInputConfig.h"
#include "MyLyra/MyLyraLogChannels.h"

UMyLyraInputConfig::UMyLyraInputConfig(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

const UInputAction* UMyLyraInputConfig::FindNativeInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound) const
{
	// NativeInputAction을 순회하며, Input으로 들어온 InputTag가 있는지 체크
	// - 있으면, 그에 따른 InputAction을 반환, 없다면 nullptr 반환
	for (const FMyLyraInputAction& Action : NativeInputActions)
	{
		if (Action.InputAction && (Action.InputTag == InputTag))
		{
			return Action.InputAction;
		}
	}

	if (bLogNotFound)
	{
		UE_LOG(LogMyLyra, Error, TEXT("can not find NativeInputAction for inputTag [%s] on InputConfig [%s]."), *InputTag.ToString(), *GetNameSafe(this));
	}

	return nullptr;
}

const UInputAction* UMyLyraInputConfig::FindAbilityInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound) const
{
	for (const FMyLyraInputAction& Action : AbilityInputActions)
	{
		if (Action.InputAction && (Action.InputTag == InputTag))
		{
			return Action.InputAction;
		}
	}

	if (bLogNotFound)
	{
		UE_LOG(LogMyLyra, Error, TEXT("can not find AbilityInputAction for inputTag [%s] on InputConfig [%s]."), *InputTag.ToString(), *GetNameSafe(this));
	}

	return nullptr;
}
