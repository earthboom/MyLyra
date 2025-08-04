// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "MyLyraGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class MYLYRA_API UMyLyraGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UMyLyraGameplayAbility(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};
