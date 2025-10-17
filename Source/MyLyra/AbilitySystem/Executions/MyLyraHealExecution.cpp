// Fill out your copyright notice in the Description page of Project Settings.

#include "MyLyraHealExecution.h"
#include "MyLyra/AbilitySystem/Attributes/MyLyraCombatSet.h"

/**
 * 해당 Struct를 사용하여, FGameplayEffectAttributeCaptureDefinition 인스턴화하여 전달
 */
struct FHealStatics
{
	/** AttributeSet의 어떤 Attribute을 Captuure할 것인지와 어떻게 Capture할지 정의를 담고 있음 */
	FGameplayEffectAttributeCaptureDefinition BaseHealDef;

	FHealStatics()
	{
		BaseHealDef = FGameplayEffectAttributeCaptureDefinition(UMyLyraCombatSet::GetBaseHealAttribute(), EGameplayEffectAttributeCaptureSource::Source, true);
	}
};

static FHealStatics& HealStatics()
{
	/** 계속 FHealStatics를 생성하는 것은 부하이니, 한번만 생성하고 재사용 */
	static FHealStatics Statics;
	return Statics;
};

UMyLyraHealExecution::UMyLyraHealExecution()
	: Super()
{
	/** Source로 (입렧값) Attribute의 캡처를 정의
	 *	- CombatSet::BaseHeal을 통해 Healing 값을 정의하고 최종 Execute할 때, 해당 값을 가져와 Health에 Healing을 적용
	 */
	RelevantAttributesToCapture.Add(HealStatics().BaseHealDef);
}

void UMyLyraHealExecution::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	float BaseHeal = 0.0f;
	{
		FAggregatorEvaluateParameters EvaluateParameters;

		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(HealStatics().BaseHealDef, EvaluateParameters, BaseHeal);
	}
}
