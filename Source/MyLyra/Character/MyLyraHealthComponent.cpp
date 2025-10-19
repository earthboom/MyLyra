// Fill out your copyright notice in the Description page of Project Settings.

#include "MyLyraHealthComponent.h"

UMyLyraHealthComponent::UMyLyraHealthComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// HealthComponent는 PlayerState의 HealthSet과 Character(Pawn) 사이의 Bridge역할
	// - 따로 로직이 업데이트 될 필요가 없는 이벤트 기반으로 동작하는 컴포넌트로 이해할 것
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;

	// InitializeWithAbilitySystem으로 ASC가 초기화되기 전까지 HealthSet과 ASC는 null
	AbilitySystemComponent = nullptr;
	HealthSet = nullptr;
}

void UMyLyraHealthComponent::InitializeWithAbilitySystem(UMyLyraAbilitySystemComponent* InASC)
{
}

void UMyLyraHealthComponent::UninitializeWithAbilitySystem()
{
}
