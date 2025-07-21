// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ModularPlayerController.h"
#include "MyLyraPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class MYLYRA_API AMyLyraPlayerController : public AModularPlayerController
{
	GENERATED_BODY()

public:
	AMyLyraPlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};
