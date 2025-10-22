// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyLyraAbilityCost.h"
#include "MyLyraAbilityCost_ItemTagStack.generated.h"

/**
 * 
 */
UCLASS()
class MYLYRA_API UMyLyraAbilityCost_ItemTagStack : public UMyLyraAbilityCost
{
	GENERATED_BODY()
public:
	UMyLyraAbilityCost_ItemTagStack();

	virtual bool CheckCost(const UMyLyraGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	virtual void ApplyCost(const UMyLyraGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const override;

	/**
	 * - FScalableFloat는 곡선으로 스케일링 됨
	 * - 곡선은 일반적으로 능력 수준에 따라 인덱싱
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Costs)
	FScalableFloat Quantity;

	// 비용과 연결된 Tag
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Costs)
	FGameplayTag Tag;

	// 실패 사유를 알려주는 Tag
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Costs)
	FGameplayTag FailureTag;
};
