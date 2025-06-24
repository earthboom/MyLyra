// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MyLyraPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class MYLYRA_API AMyLyraPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AMyLyraPlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};
