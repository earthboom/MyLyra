// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "MyLyraGameState.generated.h"

class UMyLyraExperienceManagerComponent;
/**
 * 
 */
UCLASS()
class MYLYRA_API AMyLyraGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	AMyLyraGameState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	UPROPERTY()
	TObjectPtr<UMyLyraExperienceManagerComponent> ExperienceManagerComponent = nullptr;
};
