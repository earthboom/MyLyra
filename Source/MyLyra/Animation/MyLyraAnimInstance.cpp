// Fill out your copyright notice in the Description page of Project Settings.

#include "MyLyraAnimInstance.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"

void UMyLyraAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	AActor* OwningActor = GetOwningActor();
	if (IsValid(OwningActor))
	{
		UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OwningActor);
		if (IsValid(ASC))
		{
			InitializeWithAbilitySystem(ASC);
		}
	}
}

void UMyLyraAnimInstance::InitializeWithAbilitySystem(UAbilitySystemComponent* ASC)
{
	// ASC 내부 관리하는 GameplayTag와 AnimInstance의 멤버 Property와 Delegate를 연결해, 값 변화에 대한 반영을 진행
	GameplayTagPropertyMap.Initialize(this, ASC);
}
