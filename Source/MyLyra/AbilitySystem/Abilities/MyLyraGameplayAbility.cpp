// Fill out your copyright notice in the Description page of Project Settings.


#include "MyLyraGameplayAbility.h"

UMyLyraGameplayAbility::UMyLyraGameplayAbility(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ActivationPolicy = EMyLyraAbilityActivationPolicy::OnInputTrigger;
}
