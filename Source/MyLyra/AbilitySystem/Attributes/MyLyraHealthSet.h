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
