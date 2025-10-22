// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "MyLyraGameplayAbility.generated.h"

UENUM(BlueprintType)
enum class EMyLyraAbilityActivationPolicy : uint8
{
	/** Input이 Trigger 되었을 경우 (Pressed / Released) */
	OnInputTrigger,
	/** Input이 Held되어 있을 경우 */
	WhileInputActive,
	/** Avatar가 생성되었을 경우, 바로 할당 */
	OnSpawn,
};

class UMyLyraAbilityCost;

/**
 * 
 */
UCLASS()
class MYLYRA_API UMyLyraGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UMyLyraGameplayAbility(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual bool CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	virtual void ApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const override;

	/** 언제 GA가 활성화될지 정책 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MyLyra|AbilityActivation")
	EMyLyraAbilityActivationPolicy ActivationPolicy;

	UPROPERTY(EditDefaultsOnly, Instanced, Category = "MyLyra|Costs")
	TArray<TObjectPtr<UMyLyraAbilityCost>> AdditionalCosts;
};
