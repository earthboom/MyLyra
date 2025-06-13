// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PawnComponent.h"
#include "Components/GameFrameworkInitStateInterface.h"
#include "MyLyraHeroComponent.generated.h"

/*
 * 카메라, 입력 등 플레이어가 제어하는 시스템의 초기화를 처리하는 Component
 */
UCLASS(Blueprintable, Meta = (BlueprintSpawnableComponent))
class MYLYRA_API UMyLyraHeroComponent : public UPawnComponent, public IGameFrameworkInitStateInterface
{
	GENERATED_BODY()

public:
	UMyLyraHeroComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};
