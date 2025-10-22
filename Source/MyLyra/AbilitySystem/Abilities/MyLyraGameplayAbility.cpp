// Fill out your copyright notice in the Description page of Project Settings.


#include "MyLyraGameplayAbility.h"

#include "MyLyraAbilityCost.h"

UMyLyraGameplayAbility::UMyLyraGameplayAbility(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ActivationPolicy = EMyLyraAbilityActivationPolicy::OnInputTrigger;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

bool UMyLyraGameplayAbility::CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (Super::CheckCost(Handle, ActorInfo, OptionalRelevantTags) == false || ActorInfo == nullptr)
	{
		return false;
	}

	// MyLyraGamePlayability에서 정의된 추가 Cost을 확인하여 GameplayAbility 기능을 활성화:
	for (TObjectPtr<UMyLyraAbilityCost> AdditionalCost : AdditionalCosts)
	{
		if (AdditionalCost != nullptr)
		{
			if (AdditionalCost->CheckCost(this, Handle, ActorInfo, OptionalRelevantTags) == false)
			{
				return false;
			}
		}
	}

	// 모든 Cost 요구사항이 충족, 준비 완료
	return true;
}

void UMyLyraGameplayAbility::ApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
{
	Super::ApplyCost(Handle, ActorInfo, ActivationInfo);
	check(ActorInfo);

	// 추가 Cost 지불
	for (TObjectPtr<UMyLyraAbilityCost> AdditionalCost : AdditionalCosts)
	{
		if (AdditionalCost != nullptr)
		{
			AdditionalCost->ApplyCost(this, Handle, ActorInfo, ActivationInfo);
		}
	}
}
