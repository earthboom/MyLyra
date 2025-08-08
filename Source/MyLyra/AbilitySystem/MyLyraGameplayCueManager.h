// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayCueManager.h"
#include "MyLyraGameplayCueManager.generated.h"

/**
 * 
 */
UCLASS()
class MYLYRA_API UMyLyraGameplayCueManager : public UGameplayCueManager
{
	GENERATED_BODY()

public:
	static UMyLyraGameplayCueManager* Get();

	UMyLyraGameplayCueManager(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	void RefreshGameplayCuePrimaryAsset();
};
