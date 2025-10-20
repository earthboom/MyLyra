// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/GameFrameworkComponent.h"
#include "MyLyraHealthComponent.generated.h"

struct FOnAttributeChangeData;
class UMyLyraHealthSet;
class UMyLyraAbilitySystemComponent;

// Health 변화 콜백을 위한 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FMyLyraHealth_AttributeChanged, UMyLyraHealthComponent*, HealthComponent, float, OldValue, float, NewValue, AActor*, Instigator);

/**
 * Character에 대해 체력관련 처리를 담당
 */
UCLASS(Blueprintable)
class MYLYRA_API UMyLyraHealthComponent : public UGameFrameworkComponent
{
	GENERATED_BODY()

public:
	UMyLyraHealthComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** Actor(보통 ACharacter/APawn)의 healthComponent를 반환 */
	UFUNCTION(BlueprintPure, Category = "MyLyra|Health")
	static UMyLyraHealthComponent* FindHealthComponent(const AActor* InActor);

	/** HealthSet의 Attribute에 접근하기 위한 BP Accessor 함수들 */
	UFUNCTION(BlueprintCallable, Category = "MyLyra|Health")
	float GetHealth() const;

	UFUNCTION(BlueprintCallable, Category = "MyLyra|Health")
	float GetMaxhealth() const;

	UFUNCTION(BlueprintCallable, Category = "MyLyra|Health")
	float GetHealthNormalized() const;

	// ASC와 HealthSet 조화
	void InitializeWithAbilitySystem(UMyLyraAbilitySystemComponent* InASC);
	void UninitializeWithAbilitySystem();

	/** ASC를 통해, HealthSet의 HealthAttribute변경이 있을 때 호출하는 메서드 (내부적으로 OnHealChanged 홀출) */
	void HandleHealthChanged(const FOnAttributeChangeData& ChargeData);

	// HealthSet에 접근하기 위한 ASC
	UPROPERTY()
	TObjectPtr<UMyLyraAbilitySystemComponent> AbilitySystemComponent;

	// 캐싱된 healthSet 레퍼런스
	UPROPERTY()
	TObjectPtr<const UMyLyraHealthSet> HealthSet;

	// Health 변화에 따른 Delegate
	UPROPERTY(BlueprintAssignable)
	FMyLyraHealth_AttributeChanged OnHealthChanged;
};
