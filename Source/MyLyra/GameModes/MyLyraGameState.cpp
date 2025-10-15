// Fill out your copyright notice in the Description page of Project Settings.


#include "MyLyraGameState.h"
#include "MyLyra/GameModes/MyLyraExperienceManagerComponent.h"

PRAGMA_DISABLE_OPTIMIZATION
AMyLyraGameState::AMyLyraGameState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ExperienceManagerComponent = CreateDefaultSubobject<UMyLyraExperienceManagerComponent>(TEXT("ExperienceManagerComponent"));
}
PRAGMA_ENABLE_OPTIMIZATION