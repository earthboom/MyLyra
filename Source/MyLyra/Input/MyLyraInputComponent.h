// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputTriggers.h"
#include "InputActionValue.h"
#include "MyLyraInputConfig.h"
#include "EnhancedInputComponent.h"
#include "MyLyraInputComponent.generated.h"

/**
 * 
 */
UCLASS()
class MYLYRA_API UMyLyraInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()

public:
	UMyLyraInputComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	template <class UserClass, typename FuncType>
	void BindNativeAction(const UMyLyraInputConfig* InputConfig, const FGameplayTag& InputTag, ETriggerEvent TriggerEvent, UserClass* Object, FuncType Func, bool bLogIfNotFound);

	template <class UserClass, typename PressedFuncType, typename ReleaseFuncType>
	void BindAbilityAction(const UMyLyraInputConfig* InputConfig, UserClass* Object, PressedFuncType PressedFunc, ReleaseFuncType ReleaseFunc, TArray<uint32>& BindHandles);
};

template <class UserClass, typename FuncType>
void UMyLyraInputComponent::BindNativeAction(const UMyLyraInputConfig* InputConfig, const FGameplayTag& InputTag, ETriggerEvent TriggerEvent, UserClass* Object, FuncType Func, bool bLogIfNotFound)
{
	check(InputConfig);

	// InputConfig는 활성화 가능한 InputAction을 담고 있음
	// - 만약 InputConfig에 없는 InputAction을 Binding시키면, nullptr을 반환하여, 바인딩에 실패함
	const UInputAction* IA = InputConfig->FindNativeInputActionForTag(InputTag, bLogIfNotFound);
	if (IsValid(IA))
	{
		BindAction(IA, TriggerEvent, Object, Func);
	}
}

template <class UserClass, typename PressedFuncType, typename ReleaseFuncType>
void UMyLyraInputComponent::BindAbilityAction(const UMyLyraInputConfig* InputConfig, UserClass* Object, PressedFuncType PressedFunc, ReleaseFuncType ReleaseFunc, TArray<uint32>& BindHandles)
{
	check(InputConfig);

	// AbilityAction에 대해선, 그냥 모든 InputAction에 바인딩!
	for (const FMyLyraInputAction& Action : InputConfig->AbilityInputActions)
	{
		if (Action.InputAction && Action.InputTag.IsValid())
		{
			if (PressedFunc)
			{
				BindHandles.Add(BindAction(Action.InputAction, ETriggerEvent::Triggered, Object, PressedFunc, Action.InputTag).GetHandle());
			}

			if (ReleaseFunc)
			{
				BindHandles.Add(BindAction(Action.InputAction, ETriggerEvent::Completed, Object, ReleaseFunc, Action.InputTag).GetHandle());
			}
		}
	}
}
