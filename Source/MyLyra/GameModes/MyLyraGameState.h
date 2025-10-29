// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "MyLyraGameState.generated.h"

class UMyLyraExperienceManagerComponent;

/**
 * GameState
 * - 현재 상태 정보를 캐싱하고 있는 객체로 이해
 * 
 * 보통 일반적으로 MyLyraGameMode와 MyLyraGameState는 1:1 대응이 될 것이다.
 * - Lyra의 경우엔, 게임 모드 안에 여러가지 게임이 있고, 이를 Experience로 대응
 * - 이 경우, Experience 전환을 MyLyraExperienceManagerComponent가 대응
 */
UCLASS()
class MYLYRA_API AMyLyraGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	AMyLyraGameState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	// GameState가 Owner로서 ExperienceManagerComponent를 소유함
	UPROPERTY()
	TObjectPtr<UMyLyraExperienceManagerComponent> ExperienceManagerComponent = nullptr;
};
