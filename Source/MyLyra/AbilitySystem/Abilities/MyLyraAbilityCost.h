// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilitySpec.h"
#include "GameplayAbilitySpecHandle.h"
#include "UObject/NoExportTypes.h"
#include "MyLyraAbilityCost.generated.h"

class UMyLyraGameplayAbility;
/**
 * 
 */
UCLASS(DefaultToInstanced, EditInlineNew, Abstract)
class MYLYRA_API UMyLyraAbilityCost : public UObject
{
	GENERATED_BODY()

public:
	UMyLyraAbilityCost();

	virtual bool CheckCost(const UMyLyraGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const
	{
		return true;
	}

	virtual void ApplyCost(const UMyLyraGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
	{
	}
};
