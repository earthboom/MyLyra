// Fill out your copyright notice in the Description page of Project Settings.

#include "MyLyraUserFacingExperience.h"
#include "CommonSessionSubsystem.h"

UCommonSession_HostSessionRequest* UMyLyraUserFacingExperience::CreateHostingRequest() const
{
	const FString ExperienceName = ExperienceID.PrimaryAssetName.ToString();

	// - UCommonSession_HostSessionRequest는 UObject로 생성해 놓고, 앞아서 GC가 됨
	// - 해당 객체는 현재 프레임에서 사용하기 때문에, GC에 대한 염려가 필요 업음
	// - 만약 다음 프레임이든 추가적인 프레임 상에서 해당 객체를 사용할 경우, Lifetime 관리 필요!
	// - 그렇지 않으면 Dangling
	UCommonSession_HostSessionRequest* Result = NewObject<UCommonSession_HostSessionRequest>();
	Result->MapID = MapID;
	Result->ExtraArgs.Emplace(TEXT("Experience"), ExperienceName);

	return Result;
}
