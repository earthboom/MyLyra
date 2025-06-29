// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "MyLyraMappableConfigPair.generated.h"

class UPlayerMappableInputConfig;

USTRUCT()
struct FMyLyraMappableConfigPair
{
	GENERATED_BODY()
public:
	FMyLyraMappableConfigPair() = default;

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UPlayerMappableInputConfig> Config;

	UPROPERTY(EditAnywhere)
	bool bShouldActivateAutomatically = true;
};