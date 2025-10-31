// Fill out your copyright notice in the Description page of Project Settings.

#include "MyLyraAssetManager.h"

#include "MyLyra/MyLyraGameplayTags.h"
#include "MyLyra/MyLyraLogChannels.h"

UMyLyraAssetManager::UMyLyraAssetManager()
	: Super()
{
}

UMyLyraAssetManager& UMyLyraAssetManager::Get()
{
	check(GEngine);

	// singleton pattern : 유일성을 보장하는 패턴, 즉 한 개 이상일 수 없음
	// 엔진에 캐싱된 AssetManager 를 변환하여 반환
	if (UMyLyraAssetManager* Singleton = Cast<UMyLyraAssetManager>(GEngine->AssetManager))
	{
		return *Singleton;
	}

	UE_LOG(LogMyLyra, Fatal, TEXT("Invalid AssetManagerClassname in DefaultEngin.ini (project settings); It must be MyLyraAssetManager"));

	// 위 로그 UE_LOG의 Fatal 로 인해, Crash 나기 때문에 도달하지 않으나 컴파일을 위해 더미로 리턴함
	return *NewObject<UMyLyraAssetManager>();
}

// 해당 부분에 대한 최적화를 하지 않게 함
// 원하는 부분만 디버깅이 가능하도록 할 수 있음
// Develop 일 때도 디버깅 잡힘
PRAGMA_DISABLE_OPTIMIZATION
void UMyLyraAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();

	UE_LOG(LogMyLyra, Display, TEXT("UMyLyraAssetManager::StartInitialLoading"));

	FMyLyraGameplayTags::InitializeNativeTags();
}
PRAGMA_ENABLE_OPTIMIZATION

bool UMyLyraAssetManager::ShouldLogAssetLoads()
{
	// cmd 의 command 적용과 같음 ( ex : UnrealEngeine.exe SkilCompile LogASsetLoads
	static bool bLogAssetLoads = FParse::Param(FCommandLine::Get(), TEXT("LogAssetLoads"));
	return bLogAssetLoads;
}

// 동기식 로딩 지원
// 시간 차를 구하기 위한 동기 로딩 동작을 매핑한 함수라고 할 수 있음
UObject* UMyLyraAssetManager::SynchronousLoadAsset(const FSoftObjectPath& AssetPath)
{
	// 해당 함수를 따로 만드는 이유는 'synchronous load asset 이 불필요하게 하는 것이 없는지 확인하기 위함
	if (AssetPath.IsValid())
	{
		// 로딩 시간을 측정하고 로그를 남기기 위한 'FScopeLogTime' 객체를 담을 TUniquePtr 선언
		TUniquePtr<FScopeLogTime> LogTimePtr;
		/** Asset Loading 로그를 남겨야 하는지 확인하는 조건문 
		 * - ShouldLogAssetLoads() 함수는 보통 "-LogAssetLoads" 같은 커맨드 라인 인자가 주어졌을 때, true를 반환
		 * - 개발 중 성능 문제 분석할 때, 특정 에셋의 로딩 시간을 추적하기 위한 디버깅 기능
		 */
		if (ShouldLogAssetLoads())
		{
			// 단순 로깅하면서 초단위로 로깅 진행
			LogTimePtr = MakeUnique<FScopeLogTime>(*FString::Printf(TEXT("synchronous loaded assets [%s]"), *AssetPath.ToString()), nullptr, FScopeLogTime::ScopeLog_Seconds);
		}

		// AssetManager가 유효하면 AssetManager 의 StreamableManager를 통한 정적 로딩 수행
		// FSoftobjectPath 를 통해 바로 정적 로딩
		if (UAssetManager::IsValid())
		{
			// StreamableManager 은 로딩을 관리하는 클래스
			return UAssetManager::GetStreamableManager().LoadSynchronous(AssetPath);
		}

		// 만일 AssetManager 가 준비가 안된 상황이면 TryLoad 를 사용, TryLoad는 되도록이면 쓰지 말아야 함
		// LoadSynchronous, TryLoad 둘 다 이미 로드한 에셋이라면 캐싱한 것을 가져온다 
		return AssetPath.TryLoad();
	}

	return nullptr;
}

// 언리얼에서 멀티 스레드 코딩 환경을 제공해서 평소엔 고민하지 않아도 될 정도
// 언리얼 에디터가 로딩이 끝나지 았을 때(완전히 켜지지 않았을 때)는 스레드가 Safe 하지 않음
// 그래서 Lock 이 필요하게 됨
void UMyLyraAssetManager::AddLoadedAsset(const UObject* Asset)
{
	if (ensureAlways(Asset))
	{
		// 해당 타이밍에는 멀티 스레드가 안전하다는 보장이 없기 때문에 LOCK을 건다. (Thread-safe)
		// 즉, 한 명씩 추가하겠다는 의미
		FScopeLock Lock(&SyncObject);
		LoadedAssets.Add(Asset);
	}
}
