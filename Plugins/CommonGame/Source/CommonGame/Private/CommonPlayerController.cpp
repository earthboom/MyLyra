// Fill out your copyright notice in the Description page of Project Settings.

#include "CommonPlayerController.h"
#include "CommonLocalPlayer.h"

ACommonPlayerController::ACommonPlayerController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void ACommonPlayerController::ReceivedPlayer()
{
	// PlayerController가 LocalPlayer에 붙으면서 활성화되는 이벤트 함수가 ReceivedPlayer!!
	Super::ReceivedPlayer();

	if (UCommonLocalPlayer* LocalPlayer = Cast<UCommonLocalPlayer>(Player))
	{
		LocalPlayer->OnPlayerControllerSet.Broadcast(LocalPlayer, this);
		if (PlayerState)
		{
			LocalPlayer->OnPlayerStateSet.Broadcast(LocalPlayer, PlayerState);
		}
	}
}
