// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "CommonGameInstance.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class COMMONGAME_API UCommonGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UCommonGameInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	virtual int32 AddLocalPlayer(ULocalPlayer* NewPlayer, int32 ControllerId) override;
	virtual bool RemoveLocalPlayer(ULocalPlayer* ExistingPlayer) override;

	/** 메인 Local Player를 캐싱 */
	TWeakObjectPtr<ULocalPlayer> PrimaryPlayer;
};
