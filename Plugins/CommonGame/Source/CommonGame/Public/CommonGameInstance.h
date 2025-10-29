// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "CommonGameInstance.generated.h"

/**
 * UGameInstance
 * - 게임 에플리케이션의 전체 생명 주기 동안 유지되는 단일 인스턴스
 * - 레벨(맵)이 변경되어도 파괴되지 않고 계속 존재하며, 게임 전반에 걸쳐 유지되어야 하는 글로벌 데이터나 시스템을 관리하는데 사용
 * - 게임 프로세스 당 하나로 전역적으로 정말 유니크한 객체라고 생각
 */
UCLASS(Abstract)
class COMMONGAME_API UCommonGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UCommonGameInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	virtual int32 AddLocalPlayer(ULocalPlayer* NewPlayer, FPlatformUserId UserId) override;
	virtual bool RemoveLocalPlayer(ULocalPlayer* ExistingPlayer) override;

	/** 메인 Local Player를 캐싱 */
	TWeakObjectPtr<ULocalPlayer> PrimaryPlayer;
};
