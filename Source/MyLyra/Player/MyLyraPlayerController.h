// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonPlayerController.h"
#include "MyLyraPlayerController.generated.h"

class UMyLyraAbilitySystemComponent;
class AMyLyraPlayerState;
/**
 * GameMode와 PlayerController는 상하 관계를 맺고 있다.
 * - GameMode가 PlayerController의 Spawn과 Despawn을 관장함
 */
UCLASS()
class MYLYRA_API AMyLyraPlayerController : public ACommonPlayerController
{
	GENERATED_BODY()

public:
	AMyLyraPlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** PlayerController Interface */
	virtual void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;

	AMyLyraPlayerState* GetMyLyraPlayerState() const;
	UMyLyraAbilitySystemComponent* GetMyLyraAbilitySystemComponent() const;
};

/**
 * PlayerController와 Character의 관계
 * - 동적으로 PlayerController는 Character나 Pawn에 대해 소유권을 변경할 수 있다.
 * - 게임 로직적인 관점에선, Controller와 View의 관점으로 이해하면 편함
 * -  플레이어가 오른쪽 키를 누르면(Controller), 캐릭터가 오른쪽으로 움직인다.(View)
 *
 * PlayerController와 PlayerState의 관계
 * - 간접적 소유 관계 (참조 관계)
 * - Spawn을 통해 PlayerState를 Controller가 생성
 * - Controller와 Model 관계로 이해하면 편함 (혹은 GameMode - GameState 관계)
 * - 로직(Controller)와 Model(데이터/상태) 관계로 이해
 */