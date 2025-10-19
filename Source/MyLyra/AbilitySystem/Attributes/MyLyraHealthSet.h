// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyLyraAttributeSet.h"
#include "AbilitySystemComponent.h"
#include "MyLyraHealthSet.generated.h"

/**
 * 
 */
UCLASS()
class MYLYRA_API UMyLyraHealthSet : public UMyLyraAttributeSet
{
	GENERATED_BODY()

public:
	UMyLyraHealthSet();
	
	/**
	 * MyLyraAttributeSet에 정의한, ATTRIBUTE_ACCESSORS 를 통해, 아래 정의한 멤버변수와 똑같이 이름 설정
	 */
	ATTRIBUTE_ACCESSORS(UMyLyraHealthSet, Health);
	ATTRIBUTE_ACCESSORS(UMyLyraHealthSet, MaxHealth);
	ATTRIBUTE_ACCESSORS(UMyLyraHealthSet, Healing);

	/**
	 * Attribute의 값을 ClampAttribute()를 활용해, 값의 범위를 유지시켜주기 위해
	 * PreAttributeBaseChange와 PreAttributeChange 오버라이드
	 */
	void ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const;
	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	/**
	 * GameplayEffect가 healthSet의 Attribute를 수정하기 전에 불리는 콜백함수
	 * - 이는 Healing이 업데이트되면, Health를 Healing을 적용해 업데이트 가능
	 */
	virtual bool PreGameplayEffectExecute(struct FGameplayEffectModCallbackData& Data) override;
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;
	
	/** 현재 체력 */
	UPROPERTY(BlueprintReadOnly, Category = "MyLyra|Health")
	FGameplayAttributeData Health;

	/** 체력 최대치*/
	UPROPERTY(BlueprintReadOnly, Category = "MyLyra|Health")
	FGameplayAttributeData MaxHealth;

	/** 체력 회복치 */
	UPROPERTY(BlueprintReadOnly, Category = "MyLyra|Health")
	FGameplayAttributeData Healing;
};
