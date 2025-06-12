// Fill out your copyright notice in the Description page of Project Settings.


#include "MyLyraGameState.h"
#include "MyLyra/GameModes/MyLyraExperienceManagerComponent.h"

AMyLyraGameState::AMyLyraGameState()
{
	ExperienceManagerComponent = CreateDefaultSubobject<UMyLyraExperienceManagerComponent>(TEXT("ExperienceManagerComponent"));
}
