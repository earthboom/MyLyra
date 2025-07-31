// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/GameStateComponent.h"
#include "GameFeaturePluginOperationResult.h"
#include "MyLyraExperienceManagerComponent.generated.h"

class UMyLyraExperienceDefinition;

enum class EMyLyraExperienceLoadState
{
	Unloaded,
	Loading,
	LoadingGameFeatures,
	ExecutingActions,
	Loaded,
	Deactivating,
};

DECLARE_MULTICAST_DELEGATE_OneParam(FOnMyLyraExperienceLoaded, const UMyLyraExperienceDefinition*);

/**
 * UGameStateComponent : GameState 에 붙일 수 있는 부품
 *
 * UMyLyraExperienceManagerComponent
 * - 해당 Component는 Game State를 Owner 로 가지며, Experience의 상태 정보를 가지고 있는 Component
 * - Manager 라는 단어가 포함되어 있듯, Experience 로딩 상태 업데이트 및 이벤트 관리
 */
UCLASS()
class MYLYRA_API UMyLyraExperienceManagerComponent : public UGameStateComponent
{
	GENERATED_BODY()

public:
	bool IsExperienceLoaded() { return (LoadState == EMyLyraExperienceLoadState::Loaded) && (CurrentExperience != nullptr); }

	// 아래의 OnExperienceLvlel 에 바인딩하거나, 이미 Experience 로딩이 완료되었다면 바로 호출
	void CallOrRegister_OnExperienceLoaded(FOnMyLyraExperienceLoaded::FDelegate&& Delegate);

	void ServerSetCurrentExperience(FPrimaryAssetId ExperienceId);
	void StartExperienceLoad();
	void OnExperienceLoadComplete();
	void OnGameFeaturePluginLoadComplete(const UE::GameFeatures::FResult& InResult);
	void OnExperienceFullLoadComplete();
	const UMyLyraExperienceDefinition* GetCurrentExperienceChecked() const;

public:
	UPROPERTY()
	TObjectPtr<const UMyLyraExperienceDefinition> CurrentExperience = nullptr;

	// Experience의 로딩 상태 모니터링
	EMyLyraExperienceLoadState LoadState = EMyLyraExperienceLoadState::Unloaded;

	// Experience 로딩이 완료된 이후, Broadcasting Delegate
	FOnMyLyraExperienceLoaded OnExperienceLoaded;

	/** 활성화된 GameFeature Plugin 들 */
	int32 NumGameFeaturePluginLoading = 0;
	TArray<FString> GameFeaturePluginURLs;
};
