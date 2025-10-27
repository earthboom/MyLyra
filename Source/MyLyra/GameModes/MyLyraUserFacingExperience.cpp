// Fill out your copyright notice in the Description page of Project Settings.

#include "MyLyraUserFacingExperience.h"
#include "CommonSessionSubsystem.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(MyLyraUserFacingExperience)
/**
 * UE_INLINE_GENERATED_CPP_BY_NAME
 * - cpp파일에서 사용되며, 헤더 파일에 있는 GENERATED_BODY() 매크로와 한쌍으로 작동
 * - UHT(Unreal Header Tool)가 자동으로 생성해 준 C++ 코드 구현부(.gen.cpp)를 이 자리에 통째로 붙여 넣어라라는 #include 명령어
 */


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
