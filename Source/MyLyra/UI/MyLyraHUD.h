// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/HUD.h"
#include "MyLyraHUD.generated.h"

/**
 * 
 */
UCLASS()
class MYLYRA_API AMyLyraHUD : public AHUD
{
	GENERATED_BODY()

public:
	AMyLyraHUD(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void PreInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};
