// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/GameFrameworkComponent.h"
#include "MyLyraHealthComponent.generated.h"

class UMyLyraHealthSet;
class UMyLyraAbilitySystemComponent;

// Health 변화 콜백을 위한 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FMyLYraHealth_AttributeChanged, UMyLyraHealthComponent*, HealthComponent, float, OldValue, float, NewValue, AActor*, Instigator);

/**
 * Character에 대해 체력관련 처리를 담당
 */
UCLASS(Blueprintable)
class MYLYRA_API UMyLyraHealthComponent : public UGameFrameworkComponent
{
	GENERATED_BODY()

public:
	UMyLyraHealthComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// ASC와 HealthSet 조화
	void InitializeWithAbilitySystem(UMyLyraAbilitySystemComponent* InASC);
	void UninitializeWithAbilitySystem();

	// HealthSet에 접근하기 위한 ASC
	UPROPERTY()
	TObjectPtr<UMyLyraAbilitySystemComponent> AbilitySystemComponent;

	// 캐싱된 healthSet 레퍼런스
	UPROPERTY()
	TObjectPtr<const UMyLyraHealthSet> HealthSet;

	// Health 변화에 따른 Delegate
	FMyLYraHealth_AttributeChanged OnHealthChanged;
};
