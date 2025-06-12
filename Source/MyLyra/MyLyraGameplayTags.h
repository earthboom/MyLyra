// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Containers/UnrealString.h"
#include "Containers/Map.h"
#include "GameplayTagContainer.h"
#include "NativeGameplayTags.h"

class UGameplayTagsManager;

struct FMyLyraGameplayTags
{
	static const FMyLyraGameplayTags& Get() { return GameplayTags; }
	static void InitializeNativeTags();

	void AddTag(FGameplayTag& OutTag, const ANSICHAR* TagName, const ANSICHAR* TagComment);
	void AddAllTags(UGameplayTagsManager& Manager);

	/*
	 * 아래의 GameplayTag 는 초기화 과정 단계를 의미
	 * - GameInstance의 초기화 과정에 UGameFrameworkComponentManager의 RegisterInitState로 등록되어 선형적으로 (linear)하기 업데이트 됨
	 * - 이 초기화 GameplayTag는 게임 Actor 사이에 공유되며, GameFrameworkInitStateInterface를 상속받은 클래스는 초기화 상태(Init State)를 상태머신과 같이 관리 가능한 인터페이스를 제공함
	 */
	FGameplayTag InitState_Spawned;
	FGameplayTag InitState_DataAvailable;
	FGameplayTag InitState_DataInitialized;
	FGameplayTag InitState_GameplayReady;

private:
	static FMyLyraGameplayTags GameplayTags;
};