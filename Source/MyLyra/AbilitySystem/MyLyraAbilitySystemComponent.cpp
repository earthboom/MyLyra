// Fill out your copyright notice in the Description page of Project Settings.

#include "MyLyraAbilitySystemComponent.h"
#include "Abilities/MyLyraGameplayAbility.h"

UMyLyraAbilitySystemComponent::UMyLyraAbilitySystemComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UMyLyraAbilitySystemComponent::AbilityInputTagPressed(const FGameplayTag& InputTag)
{
	if (InputTag.IsValid())
	{
		// 허용된 GameplayAbilitySpec 순회
		for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
		{
			// Ability 가 존재, DynamicAbilityTags에 InputTag가 있는 경우, InputPressed, Held에 넣어 Ability 처리 대기
			const FGameplayTagContainer& DynamicSpecSourceTags = AbilitySpec.GetDynamicSpecSourceTags();
			if (IsValid(AbilitySpec.Ability) && (DynamicSpecSourceTags.HasTagExact(InputTag)))
			{
				InputPressedSpecHandles.AddUnique(AbilitySpec.Handle);
				InputHeldSpecHandles.AddUnique(AbilitySpec.Handle);
			}
		}
	}
}

void UMyLyraAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	if (InputTag.IsValid())
	{
		// 허용된 GameplayAbilitySpec 순회
		for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
		{
			const FGameplayTagContainer& DynamicSpecSourceTags = AbilitySpec.GetDynamicSpecSourceTags();
			if (IsValid(AbilitySpec.Ability) && (DynamicSpecSourceTags.HasTagExact(InputTag)))
			{
				// Released에는 추가, Held에선 제거
				InputReleasedSpecHandles.AddUnique(AbilitySpec.Handle);
				InputHeldSpecHandles.Remove(AbilitySpec.Handle);
			}
		}
	}
}

void UMyLyraAbilitySystemComponent::ProcessAbilityInput(float DeltaTime, bool bGamePaused)
{
	TArray<FGameplayAbilitySpecHandle> AbilitiesToActivate;

	// InputHeldSpecHandles에 대해 Ability 처리를 위해 AbilitiesToActivate에 추가
	for (const FGameplayAbilitySpecHandle& SpecHandle : InputPressedSpecHandles)
	{
		// FindAbilitySpecFromHandle
		// - ActivatableAbilities의 Handle 값 비교를 통해 GameplayAbilitySpec 반환
		const FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle);
		if (AbilitySpec)
		{
			if (IsValid(AbilitySpec->Ability) && AbilitySpec->IsActive() == false)
			{
				const UMyLyraGameplayAbility* MyLyraAbilityCDO = CastChecked<UMyLyraGameplayAbility>(AbilitySpec->Ability);

				// ActivatePolicy 가 WhileInputActivate 속성이면 활성화로 등록
				if (MyLyraAbilityCDO->ActivationPolicy == EMyLyraAbilityActivationPolicy::WhileInputActive)
				{
					AbilitiesToActivate.AddUnique(AbilitySpec->Handle);
				}
			}
		}
	}

	for (const FGameplayAbilitySpecHandle& SpecHandle : InputPressedSpecHandles)
	{
		FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle);
		if (AbilitySpec)
		{
			if (IsValid(AbilitySpec->Ability))
			{
				AbilitySpec->InputPressed = true;

				if (AbilitySpec->IsActive())
				{
					// 이미 Ability가 활성화되어 있을 경우, Input Event (InputPressed)만 호출
					AbilitySpecInputPressed(*AbilitySpec);
				}
				else
				{
					const UMyLyraGameplayAbility* MyLyraAbilityCDO = CastChecked<UMyLyraGameplayAbility>(AbilitySpec->Ability);

					// ActivatePolicy 가 OnInputTrigger 속성이면 활성화로 등록
					if (MyLyraAbilityCDO->ActivationPolicy == EMyLyraAbilityActivationPolicy::OnInputTrigger)
					{
						AbilitiesToActivate.AddUnique(AbilitySpec->Handle);
					}
				}
			}
		}
	}

	// 등록된 AbilitiesToActivate를 한 번에 등록 시작
	for (const FGameplayAbilitySpecHandle& AbilitySpecHandle : AbilitiesToActivate)
	{
		// CallActivate 호출로 BP의 Activate 노드가 실행될 것
		TryActivateAbility(AbilitySpecHandle);
	}

	// 이번 프레임에 Release되었다면, 고나련 GameplayAbility 처리
	for (const FGameplayAbilitySpecHandle& SpecHandle : InputReleasedSpecHandles)
	{
		FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle);
		if (AbilitySpec)
		{
			if (IsValid(AbilitySpec->Ability))
			{
				AbilitySpec->InputPressed = false;

				if (AbilitySpec->IsActive())
				{
					AbilitySpecInputReleased(*AbilitySpec);
				}
			}
		}
	}

	// InputHeldSpecHandles는 InputReleasedSpecHandles 추가될 때 제거
	InputPressedSpecHandles.Reset();
	InputReleasedSpecHandles.Reset();
}
