// Fill out your copyright notice in the Description page of Project Settings.

#include "CommonPlayerController.h"
#include "CommonLocalPlayer.h"

void ACommonPlayerController::ReceivedPlayer()
{
	// PlayerController가 LocalPlayer에 붙으면서 활성화되는 이벤트 함수가 ReceivedPlayer!!
	Super::ReceivedPlayer();

	UCommonLocalPlayer* LocalPlayer = Cast<UCommonLocalPlayer>(Player);
	if (IsValid(LocalPlayer))
	{
		LocalPlayer->OnPlayerControllerSet.Broadcast(LocalPlayer, this);
	}
}
