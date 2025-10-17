// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "MyLyraHealExecution.generated.h"

/**
 * UMyLyraHealExecution은 GameplayEffect의 Execution을 사용자 정의에 따라 GameplayEffect 처리를 할 수 있음
 * - HealExecution 이름에서 드러나듯, HealthAttribute의 Healing을 적용
 */
UCLASS()
class MYLYRA_API UMyLyraHealExecution : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
	
public:
	UMyLyraHealExecution();

	/** UGameplayEffectExecutionCalculation의 Execute() BlueprintNativeEvent를 오버라이드 함 */
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
