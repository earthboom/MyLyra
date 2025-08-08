// Fill out your copyright notice in the Description page of Project Settings.

#include "MyLyraGameFeaturePolicy.h"
#include "GameFeatureAction.h"
#include "GameFeatureAction_AddGameplayCuePath.h"
#include "GameFeatureData.h"
#include "GameplayCueSet.h"
#include "MyLyra/AbilitySystem/MyLyraGameplayCueManager.h"

UMyLyraGameFeaturePolicy::UMyLyraGameFeaturePolicy(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UMyLyraGameFeaturePolicy::InitGameFeatureManager()
{
	// GameFeature_AddGameplayCuePaths 등록
	Observers.Emplace(NewObject<UMyLyraGameFeature_AddGameplayCuePaths>());

	// Observers를 순회하며, GameFeaturesSubsystem에 Observers를 순회하며 등록
	UGameFeaturesSubsystem& Subsystem = UGameFeaturesSubsystem::Get();
	for (UObject* Observer : Observers)
	{
		Subsystem.AddObserver(Observer);
	}

	// 앞서, Subsystem에 이미 Observer를 등록했으므로, GaemeFeatureManager가 초기화되는 과정에 반영될 것
	Super::InitGameFeatureManager();
}

void UMyLyraGameFeaturePolicy::ShutdownGameFeatureManager()
{
	Super::ShutdownGameFeatureManager();

	UGameFeaturesSubsystem& Subsystem = UGameFeaturesSubsystem::Get();
	for (UObject* Observer : Observers)
	{
		Subsystem.RemoveObserver(Observer);
	}

	Observers.Empty();
}

void UMyLyraGameFeature_AddGameplayCuePaths::OnGameFeatureRegistering(const UGameFeatureData* GameFeatureData, const FString& PluginName, const FString& PluginURL)
{
	// PluginName 을 활용해, PluginRootPath를 계산
	const FString PluginRootPath = TEXT("/") + PluginName;

	// GameFeatureData의 Action을 순회
	// - 예) ShooterCore의 GameFeatureData에 Action을 추가했다면, 그 추가된 Action이 대상이 됨
	for (const UGameFeatureAction* Action : GameFeatureData->GetActions())
	{
		// 그 중에 _AddGameplayCuePath를 찾아 처리
		const UGameFeatureAction_AddGameplayCuePath* AddGameplayCueGFA = Cast<UGameFeatureAction_AddGameplayCuePath>(Action);
		if (IsValid(AddGameplayCueGFA))
		{
			const TArray<FDirectoryPath>& DirsToAdd = AddGameplayCueGFA->DirectoryPathsToAdd;

			// GameplayCueManager를 가져와서, GFA에 등록된 DirsToAdd를 추가하면서 GCM의 데이터가 업데이트 되도록 진행
			UMyLyraGameplayCueManager* GCM = UMyLyraGameplayCueManager::Get();
			if (IsValid(GCM))
			{
				// RuntimeCueSet을 가져옴
				UGameplayCueSet* RuntimeGameplayCueSet = GCM->GetRuntimeCueSet();
				const int32 PreInitializeNumCues = RuntimeGameplayCueSet ? RuntimeGameplayCueSet->GameplayCueData.Num() : 0;

				for (const FDirectoryPath& Directory : DirsToAdd)
				{
					FString MutablePath = Directory.Path;

					// PluginPackagePath를 한 번 보정해 줌 -> 보정 결과는 MutablePath로 들어옴
					UGameFeaturesSubsystem::FixPluginPackagePath(MutablePath, PluginRootPath, false);

					// GCM의 RuntimeGameplayCueObjectLibrary의 Path에 추가
					GCM->AddGameplayCueNotifyPath(MutablePath, false);
				}

				// 초기화 (새로 경로가 추가되었으니 호출해야 함)
				if (DirsToAdd.IsEmpty() == false)
				{
					GCM->InitializeRuntimeObjectLibrary();
				}

				// Cue의 에셋의 Pre와 Post랑 비교하여 갯수가 달라져있다면, 명시적으로 RefreshGameplayCuePrimaryAsset 호출
				const int32 PostInitializeNumCues = RuntimeGameplayCueSet ? RuntimeGameplayCueSet->GameplayCueData.Num() : 0;
				if (PreInitializeNumCues != PostInitializeNumCues)
				{
					// RefreshGameplayCuePrimaryAsset 함수는 추가된 GCN을 AssetManager에 등록해주는 역할을 해줌
					GCM->RefreshGameplayCuePrimaryAsset();
				}
			}
		}
	}
}

void UMyLyraGameFeature_AddGameplayCuePaths::OnGameFeatureUnregistering(const UGameFeatureData* GameFeatureData, const FString& PluginName, const FString& PluginURL)
{
	const FString PluginRootPath = TEXT("/") + PluginName;
	for (const UGameFeatureAction* Action : GameFeatureData->GetActions())
	{
		const UGameFeatureAction_AddGameplayCuePath* AddGameplayCueGFA = Cast<UGameFeatureAction_AddGameplayCuePath>(GameFeatureData);
		if (IsValid(AddGameplayCueGFA))
		{
			const TArray<FDirectoryPath>& DirsToAdd = AddGameplayCueGFA->DirectoryPathsToAdd;
			UMyLyraGameplayCueManager* GCM = UMyLyraGameplayCueManager::Get();
			if (IsValid(GCM))
			{
				int32 NumRemoved = 0;
				for (const FDirectoryPath& Directory : DirsToAdd)
				{
					FString MutablePath = Directory.Path;
					UGameFeaturesSubsystem::FixPluginPackagePath(MutablePath, PluginRootPath, false);
					NumRemoved += GCM->RemoveGameplayCueNotifyPath(MutablePath, false);
				}

				ensure(NumRemoved == DirsToAdd.Num());

				if (NumRemoved > 0)
				{
					GCM->InitializeRuntimeObjectLibrary();
				}
			}
		}
	}
}
