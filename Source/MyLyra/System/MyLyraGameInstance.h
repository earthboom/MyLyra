// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonGameInstance.h"
#include "Engine/GameInstance.h"
#include "MyLyraGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class MYLYRA_API UMyLyraGameInstance : public UCommonGameInstance
{
	GENERATED_BODY()

	// UGameInstance's interface
	virtual void Init() override;
	virtual void Shutdown() override;
};
