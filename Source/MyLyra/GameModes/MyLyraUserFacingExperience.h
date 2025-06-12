// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "MyLyraUserFacingExperience.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class MYLYRA_API UMyLyraUserFacingExperience : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	// the specific map to load
	// 유저가 경험할 맵을 들고 있음
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=Experience, meta=(AllowedTypes="Map"))
	FPrimaryAssetId MapID;

	// the gameplay experience to load
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=Experience, meta=(AllowedTypes="MyLyraExperienceDefinition"))
	FPrimaryAssetId ExperienceID;
};
