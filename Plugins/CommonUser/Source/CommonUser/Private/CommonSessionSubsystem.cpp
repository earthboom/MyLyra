// Fill out your copyright notice in the Description page of Project Settings.

#include "CommonSessionSubsystem.h"
#include "Engine/AssetManager.h"

#pragma region UCommonSession_HostSessionRequest
FString UCommonSession_HostSessionRequest::GetMapName() const
{
	// AssetManager를 통해, MapId를 통해, FAssetData를 반환하고, 이를 통해 PackageName을 통해 Map 경호 반환
	FAssetData MapAssetData;
	bool bIsCheck = UAssetManager::Get().GetPrimaryAssetData(MapID, MapAssetData);
	if (bIsCheck)
	{
		return MapAssetData.PackageName.ToString();
	}
	return FString();
}

FString UCommonSession_HostSessionRequest::ConstructTravelURL() const
{
	FString CombinedExtraArgs;

	for (const TPair<FString, FString>& ExtraArg : ExtraArgs)
	{
		if (ExtraArg.Key.IsEmpty())
		{
			continue;
		}

		/**
		 * separate로 복수개의 ExtraArg를 추가
		 * - Key 값 유무에 따라, =(assignment) 를 통해 알맞는  CmdArg 생성
		 */
		if (ExtraArg.Value.IsEmpty())
		{
			CombinedExtraArgs += FString::Printf(TEXT("?%s"), *ExtraArg.Key);
		}
		else
		{
			CombinedExtraArgs += FString::Printf(TEXT("?%s=%s"), *ExtraArg.Key, *ExtraArg.Value);
		}
	}

	// Map 경로 앞에 추가하여, 최종 TravelURL 생성
	return FString::Printf(TEXT("%s%s"), *GetMapName(), *CombinedExtraArgs);
}
#pragma endregion

#pragma region UCommonSessionSubsystem
void UCommonSessionSubsystem::HostSession(APlayerController* HostingPlayer, UCommonSession_HostSessionRequest* Request)
{
	ULocalPlayer* LocalPlayer = IsValid(HostingPlayer) ? HostingPlayer->GetLocalPlayer() : nullptr;
	if (IsValid(LocalPlayer) == false)
	{
		return;
	}

	// HostSessionRequest에서 MapID와 ExtraArgs를 통해 URL을 생성해, MapLoad를 시작
	// ex) TestMap?Experience=FPSMode
	GetWorld()->ServerTravel(Request->ConstructTravelURL());
}
#pragma endregion
