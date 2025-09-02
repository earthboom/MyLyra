// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/LocalPlayer.h"
#include "CommonLocalPlayer.generated.h"

/**
 * 
 */
UCLASS()
class COMMONGAME_API UCommonLocalPlayer : public ULocalPlayer
{
	GENERATED_BODY()

public:
	UCommonLocalPlayer();

	/** Player Controller가 Local Player에 할당(Assign)되었을 때 실행할 Delegate */
	DECLARE_MULTICAST_DELEGATE_TwoParams(FPlayerControllerSetDelegate, UCommonLocalPlayer* LocalPlayer, APlayerController* PlayerController)
	FPlayerControllerSetDelegate OnPlayerControllerSet;

	/** Player State가 Local Player에 할당(Assign)되었을 경위 실행할 Delegate */
	DECLARE_MULTICAST_DELEGATE_TwoParams(FPlayerStateSetDelegate, UCommonLocalPlayer* UCommonLocalPlayer, APlayerState* PlayerState)
	FPlayerStateSetDelegate OnPlayerStateSet;

	FDelegateHandle CallAndRegister_OnPlayerStateSet(FPlayerStateSetDelegate::FDelegate Delegate);
};
