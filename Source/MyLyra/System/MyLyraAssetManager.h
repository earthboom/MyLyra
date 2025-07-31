// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "MyLyraAssetManager.generated.h"

/**
 * 
 */
UCLASS()
class MYLYRA_API UMyLyraAssetManager : public UAssetManager
{
	GENERATED_BODY()

public:
	UMyLyraAssetManager();

	static UMyLyraAssetManager& Get();

	// 함수가 시작하는 시점
	virtual void StartInitialLoading() override;

	// FScopeLogTime을 사용할지 않을지 유무 판단을 위해 사용
	static bool ShouldLogAssetLoads();

	static UObject* SynchronousLoadAsset(const FSoftObjectPath& AssetPath);

	// GetAsset 은 정적 로딩으로 BP Class와 Object 로딩
	// bKeepInMemory 가 중요
	template <typename AssetType>
	static AssetType* GetAsset(const TSoftObjectPtr<AssetType>& AssetPointer, bool bKeepInMemory = true);

	template <typename AssetType>
	static TSubclassOf<AssetType> GetSubClass(const TSoftClassPtr<AssetType>& AssetPointer, bool bKeepInMemory = true);

	// [THREAD_SAFE] 메모리에 로딩된 에셋 캐싱
	void AddLoadedAsset(const UObject* Asset);

	// GC 대상
	UPROPERTY()
	TSet<TObjectPtr<const UObject>> LoadedAssets;

	// Object 단위 Locking
	FCriticalSection SyncObject;
};

template <typename AssetType>
AssetType* UMyLyraAssetManager::GetAsset(const TSoftObjectPtr<AssetType>& AssetPointer, bool bKeepInMemory)
{
	AssetType* LoadedAsset = nullptr;
	const FSoftObjectPath& AssetPath = AssetPointer.ToSoftObjectPath();
	if (AssetPath.IsValid())
	{
		// 로딩 완료일 때 바로 가져 옮
		// 로딩 안 되었을 때, nullptr 반환
		LoadedAsset = AssetPointer.Get();
		if (LoadedAsset.IsValid() == false)
		{
			// 동기 로딩 진행
			LoadedAsset = Cast<AssetType>(SynchronousLoadAsset(AssetPath));
			ensureAlwaysMsgf(LoadedAsset, TEXT("Failed to load asset [%s]"), *AssetPointer.ToString());
		}
	}

	if (LoadedAsset && bKeepInMemory)
	{
		// AddLoadedAsset 은 메모리에 상주하기 위한 장치라고 생각하면 됨
		// 한 번 등록되면 직접 내리지 않는 이상 Unload가 되지 않음 ( == caching ) 
		Get().AddLoadedAsset(Cast<UObject>(LoadedAsset));
	}

	return LoadedAsset;
}

template <typename AssetType>
TSubclassOf<AssetType> UMyLyraAssetManager::GetSubClass(const TSoftClassPtr<AssetType>& AssetPointer, bool bKeepInMemory)
{
	TSubclassOf<AssetType> LoadedSubClass;

	const FSoftObjectPath& AssetPath = AssetPointer.ToSoftObjectPath();
	if (AssetPath.IsValid())
	{
		LoadedSubClass = AssetPointer.Get();
		if (LoadedSubClass.IsValid() == false)
		{
			LoadedSubClass = Cast<UClass>(SynchronousLoadAsset(AssetPath));
			ensureAlwaysMsgf(LoadedSubClass, TEXT("Failed to load asset class [%s]"), *AssetPointer.ToString());
		}
	}

	if (LoadedSubClass && bKeepInMemory)
	{
		Get().AddLoadedAsset(Cast<UObject>(LoadedSubClass));
	}

	return LoadedSubClass;
}
