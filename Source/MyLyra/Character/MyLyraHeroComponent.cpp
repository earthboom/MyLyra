// Fill out your copyright notice in the Description page of Project Settings.

#include "MyLyraHeroComponent.h"

UMyLyraHeroComponent::UMyLyraHeroComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;
}
