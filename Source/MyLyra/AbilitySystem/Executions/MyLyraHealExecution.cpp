// Fill out your copyright notice in the Description page of Project Settings.

#include "MyLyraHealExecution.h"
#include "MyLyra/AbilitySystem/Attributes/MyLyraCombatSet.h"
#include "MyLyra/AbilitySystem/Attributes/MyLyraHealthSet.h"

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
	// GameplayEffectSpec은 GameplayEffect의 핸들로 생각하자
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	float BaseHeal = 0.0f;
	{
		FAggregatorEvaluateParameters EvaluateParameters;

		// 해당 함수 호출을 통해 MyLyraCombatSet의 BaseHeal 값을 가져옮 (혹은 뭔가 Modifier에 누적되어 있었다면, 최종 계산 결과가 나옮)
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(HealStatics().BaseHealDef, EvaluateParameters, BaseHeal);
	}

	const float HealingDone = FMath::Max(0.0f, BaseHeal);
	if (HealingDone > 0.0f)
	{
		/**
		 * GameplayEffectCalculation 이후, Modifier로서, 추가
		 * - 해당 Modifier는 CombatSet에서 가져온 BaseHeal을 활용하여, HealSet의 Healing에 추가
		 */
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UMyLyraHealthSet::GetHealingAttribute(), EGameplayModOp::Additive, HealingDone));
	}
}
