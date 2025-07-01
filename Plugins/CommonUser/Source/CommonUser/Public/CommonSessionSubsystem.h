// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "CommonSessionSubsystem.generated.h"

/**
 * UCommonSession_HostSessionRequest를 만들어 CommonSessionSubsystem에 전달하면, CommonSessionSbusystem에서 MapID, CmdArgs로 최종적인 CmdArgs를 만들어 줌
 */
UCLASS(BlueprintType)
class COMMONUSER_API UCommonSession_HostSessionRequest : public UObject
{
	GENERATED_BODY()
public:
	/** MapID -> Text 변환 */
	FString GetMapName() const;

	/** ServerTravel에 전달할 최종 URL 생성 */
	FString ConstructTravelURL() const;

	/** 준비할 MapID (맵 경로) */
	UPROPERTY(BlueprintReadWrite, Category = Session)
	FPrimaryAssetId MapID;

	/** 전달할 CmdArgs (Experience 이름을 전달) -> 곧 UserFacingExperienceDefinition에서 보게 될 것*/
	UPROPERTY(BlueprintReadWrite, Category = Session)
	TMap<FString, FString> ExtraArgs;
};

/**
 * MyLyra에 사용되어야 하는 Subsystem, Module Export를 해줘야 하고 그래서 COMMONUSER_API를 추가해야 함
 * - {ModuleName}_API의 추가는 *** 다른 모듈에서 사용할 경우, 추가해주면 됨 ***
 */
UCLASS()
class COMMONUSER_API UCommonSessionSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	UCommonSessionSubsystem(){}

	UFUNCTION(BlueprintCallable, Category = Session)
	void HostSession(APlayerController* HostingPlayer, UCommonSession_HostSessionRequest* Request);

	/** 흔히 맵의 경로로 생각하면 됨 */
	FString PendingTravelURL;
};
