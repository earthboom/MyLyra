// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyLyraAttributeSet.h"
#include "AbilitySystemComponent.h"
#include "MyLyraCombatSet.generated.h"

/**
 * 
 */
UCLASS()
class MYLYRA_API UMyLyraCombatSet : public UMyLyraAttributeSet
{
	GENERATED_BODY()
	
public:
	UMyLyraCombatSet();

	ATTRIBUTE_ACCESSORS(UMyLyraCombatSet, BaseHeal);

	/**
	 * FGameplayAttribute가 참고하는 실제 Attributes에 있는 데이터 (float보다 해당 struct를 사용하는 걸 추천)
	 * - Healing의 단위를 를 의미
	 * - e.g. 5.0f이라면, Period당 5씩 Healing된다는 의미
	 */
	UPROPERTY(BlueprintReadOnly, Category="MyLyra|Combat")
	FGameplayAttributeData BaseHeal;
};
