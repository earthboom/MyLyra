// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "MyLyraUserFacingExperience.generated.h"

class UCommonSession_HostSessionRequest;
/**
 * 
 */
UCLASS(BlueprintType)
class MYLYRA_API UMyLyraUserFacingExperience : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	/** Map 로딩 및 Experience 전환을 위해, MapID와 ExperienceID를 활용하며, HostSessionRequest 생성 */
	UFUNCTION(BlueprintCallable, BlueprintPure = false)
	UCommonSession_HostSessionRequest* CreateHostingRequest() const;

	/** the specific map to load
	 *	유저가 경험할 맵을 들고 있음
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Experience, meta = (AllowedTypes = "Map"))
	FPrimaryAssetId MapID;

	/** the gameplay experience to load */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Experience, meta = (AllowedTypes = "MyLyraExperienceDefinition"))
	FPrimaryAssetId ExperienceID;
};
