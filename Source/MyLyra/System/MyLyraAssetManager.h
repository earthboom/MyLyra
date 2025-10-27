// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "MyLyraAssetManager.generated.h"

/**
 * 프로젝트의 '총괄 자산 롼리자' 또는 '중앙 도서관 사서'에 비유
 *
 * 1. 핵심 목적 : 중앙화된 에셋 관리 및 통제
 * - 싱글톤 클래스로 게임 전체에 단 하나만 존재하고, 모든 에셋의 로딩, 언로딩, 그리고 생명 주기를 총괄
 * - UAssetManager 를 그대로 쓰지 않는 이유는 '중앙화된 통제'를 강제하기 위함
 *
 * 2. 주요 기능 및 사용 시점
 * 1) 게임 시작 시점 : 초기화의 허브 (StartInitialLoading)
 * - 게임 엔진이 시작될 때 단 한 번 호출
 * - 이 함수는 부모 클래스의 로직을 실행한 후, FMyLyraGameplayTags::InitializeNativeTags()를 호출 (매우 중요한 아키텍처적 패턴)
 * -  Asset Manager는 가장 먼저 초기화되는 시스템 중 하나이므로, 게임플레이 태그(Gameplay Tags)처럼 다른 모든 시스템보다 먼저 준비되어야 하는 핵심 데이터를 초기화하는 완벽한 '시동 지점(entry point)'
 * 2) 에셋 로딩 관리 : SynchronousLoadAsset, GetAsset
 * - 소프트 오브젝트 포인터(TSoftObjectPtr)로 지정된 애셋의 실제 데이터가 필요할 때 호출
 * - `SynchronousLoadAsset`: 이 함수는 단순히 애셋을 동기 로딩하는 것을 넘어, FScopeLogTime을 이용해 로딩 시간을 기록하고 로그를 남김
 * - `GetAsset` / `GetSubClass`: 소프트 포인터를 실제 애셋(오브젝트나 클래스)으로 변환해주는 편리한 템플릿 함수
 * 3) 메모리 관리 : AddLoadedAsset, bKeepInMemory
 * - GetAsset 함수 등에서 bKeepInMemory 파라미터가 true로 설정되었을 때 호출
 * - 로드된 애셋을 LoadedAssets라는 TSet에 추가하여 강제 참조(Hard Reference)를 만듦
 * - 이렇게 하면 가비지 컬렉터(GC)가 해당 애셋을 '아무도 사용하지 않는 애셋'으로 판단하여 메모리에서 해제하는 것을 막을 수 있음
 * - "이 애셋은 매우 중요하니, 내가 명시적으로 해제하기 전까지는 절대 메모리에서 내리지 마"라고 엔진에게 알려주는 역할
 * - 자주 사용되는 애셋을 반복적으로 로딩하는 비용을 없애줍니다.
 *
 * 결론
 * -  단순한 로더가 아닌, 안정성, 성능, 데이터 기반 철할을 지키기 위해 에셋 로딩 규칙을 가제하고, 디버깅 기능을 제공하며, 메모리를 효율적으로 관리하는 '중앙 통제 시스템'
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

		if (LoadedAsset && bKeepInMemory)
		{
			// AddLoadedAsset 은 메모리에 상주하기 위한 장치라고 생각하면 됨
			// 한 번 등록되면 직접 내리지 않는 이상 Unload가 되지 않음 ( == caching ) 
			Get().AddLoadedAsset(Cast<UObject>(LoadedAsset));
		}
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

		if (LoadedSubClass && bKeepInMemory)
		{
			Get().AddLoadedAsset(Cast<UObject>(LoadedSubClass));
		}
	}

	return LoadedSubClass;
}

/**
 * 왜 Asset 과 Subclass를 나누어 놓았을까?
 * - 정확히 Class와 Object 구분으로 이해하면 됨
 *
 * - Class는 Template 혹은 MetaData
 * - Object는 Instanced된 객체
 *
 * Ex)
 * 보통 Actor 생성은 BP Class 로딩 -> SpawnActor
 * StaticMeshComponent 생성은 StaticMesh 로딩 (AssetLoading) -> CreateObject 후, SetStaticMesh 호출
 */