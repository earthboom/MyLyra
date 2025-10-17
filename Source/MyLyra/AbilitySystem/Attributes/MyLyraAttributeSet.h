// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "MyLyraAttributeSet.generated.h"

/**
 * AttributeSet에 Attribute를 추가할 때, 선언 및 정의해야할 메서드에 대한 간략 버전 제공하는 매크로
 *
 * ex) ATTRIBUTE_ACCESSORS(UMyLyraAttributeSet, Health);
 * 아래와 같은 메서드 선언 및 정의해 줌
 *
 *	static FGameplayAttribute GetHealthAttribute() {...}
 *	float GetHealth() const {...}
 *	void SetHealth(float NewVal) {...}
 *	void InitHealth(float NewVal) {...}
 */
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 * MyLyraAttributeSet
 * - Lyra와 마찬가지로 메인 Attribute Set Class
 */
UCLASS()
class MYLYRA_API UMyLyraAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UMyLyraAttributeSet();
};
