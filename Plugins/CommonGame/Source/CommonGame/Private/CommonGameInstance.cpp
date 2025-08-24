// Fill out your copyright notice in the Description page of Project Settings.

#include "CommonGameInstance.h"
#include "CommonLocalPlayer.h"
#include "GameUIManagerSubsystem.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(CommonGameInstance)

UCommonGameInstance::UCommonGameInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

int32 UCommonGameInstance::AddLocalPlayer(ULocalPlayer* NewPlayer, int32 ControllerId)
{
	// GameInstance에 관리하는 Player 컨테이너에 추가
	int32 ReturnVal = Super::AddLocalPlayer(NewPlayer, ControllerId);
	if (ReturnVal != INDEX_NONE)
	{
		// PrimaryPlayer는 처음만 캐싱 (Index 0 을 메인으로 잡는듯?)
		if (PrimaryPlayer.IsValid() == false)
		{
			PrimaryPlayer = NewPlayer;
		}

		// GameUIManagerSubsystem을 통해 NotifyPlayerAdded() 호출로 GameLayout을 추가
		GetSubsystem<UGameUIManagerSubsystem>()->NotifyPlayerAdded(Cast<UCommonLocalPlayer>(NewPlayer));
	}

	return ReturnVal;
}

bool UCommonGameInstance::RemoveLocalPlayer(ULocalPlayer* ExistingPlayer)
{
	if (PrimaryPlayer == ExistingPlayer)
	{
		PrimaryPlayer.Reset();
	}

	GetSubsystem<UGameUIManagerSubsystem>()->NotifyPlayerDestroyed(Cast<UCommonLocalPlayer>(ExistingPlayer));
	return Super::RemoveLocalPlayer(ExistingPlayer);
}
