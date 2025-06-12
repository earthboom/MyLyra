// Fill out your copyright notice in the Description page of Project Settings.

#include "MyLyraExperienceManagerComponent.h"
#include "MyLyraExperienceDefinition.h"
#include "MyLyra/System/MyLyraAssetManager.h"
#include "GameFeaturesSubsystemSettings.h"

void UMyLyraExperienceManagerComponent::CallOrRegister_OnExperienceLoaded(FOnMyLyraExperienceLoaded::FDelegate&& Delegate)
{
	if (IsExperienceLoaded())
	{
		Delegate.Execute(CurrentExperience);
	}
	else
	{
		/**
		 * 참고
		 * Delegate 객체를 보면, 내부적으로 필요한 변수들은 메모리에 할당해 둠
		 * TArray<int> A = { 1, 2, 3, 4 }
		 * delegate_type, dolegate = [a](){
		 *		return a.bNum();
		 *	}
		 *
		 * a 는 delegate_type 내부에 new로 할당 됨
		 * 복사 비용을 낮추기 위해 Move를 통해 하는 것을 잊지 말 것	
		 */
		OnExperienceLoaded.Add(MoveTemp(Delegate));
	}
}

void UMyLyraExperienceManagerComponent::ServerSetCurrentExperience(FPrimaryAssetId ExperienceId)
{
	UMyLyraAssetManager& AssetManager = UMyLyraAssetManager::Get();

	TSubclassOf<UMyLyraExperienceDefinition> AssetClass;
	{
		// ExperienceId를 통해 경로를 가져옮
		FSoftObjectPath AssetPath = AssetManager.GetPrimaryAssetPath(ExperienceId);
		AssetClass = Cast<UClass>(AssetPath.TryLoad()); // 동기 로드
	}

	// CDO(Class Default Object)를 가져오는 이유는?
	// - GetPrimaryAssetId()를 의도대로 작동시키기 위함
	// - 의도는 UPrimaryDataAsset을 상속받는 클래스를 찾기 위해
	const UMyLyraExperienceDefinition* Experience = GetDefault<UMyLyraExperienceDefinition>(AssetClass);
	check(Experience != nullptr);
	check(CurrentExperience == nullptr);
	{
		// CDO로 CurrentExperience를 설정
		CurrentExperience = Experience;
	}

	StartExperienceLoad();
}

void UMyLyraExperienceManagerComponent::StartExperienceLoad()
{
	check(CurrentExperience);
	check(LoadState == EMyLyraExperienceLoadState::Unloaded);

	LoadState = EMyLyraExperienceLoadState::Loading;
	UMyLyraAssetManager& AssetManager = UMyLyraAssetManager::Get();

	/**
	 * ServerSetCurrentExperience에서 Experenced를 넘겨주었음, 여기서 CDO를 활용해, GetPrimaryAssetId를 로딩할 대상으로 넣음!
	 * - 왜 이렇게 하지?
	 * - GetPrimaryAssetId를 살펴봄으로써, 두 가지를 알 수 있음
	 * 1. B_MyLayraDefaultExperience를 BP로 만든 이유
	 * 2. CDO를 가져와, GetPrimaryAssetId를 호출한 이유
	 */

	// 이미 CDO로 로딩하여, CDO를 사용하지 않고 CDO를 사용하여 로딩할 에셋을 지정해, BundleAssetList에 추가해 줌	
	TSet<FPrimaryAssetId> BundleAssetList;
	BundleAssetList.Add(CurrentExperience->GetPrimaryAssetId());

	/**
	 * Load Assets associated with the experience
	 * 후일 GameFeature를 사용해, Experience에 바인됭된 GameFeature Plugin을 로딩할 Bundle 이름을 추가
	 * - Bundle 이라는 것은 나중에 로딩할 Asset의 카테고리 이름이라 생각하면 됨
	 */
	TArray<FName> BundlesToLoad;
	{
		// OwnerNetMode 가 NM_Standalone이면 ? Client / Server 둘 다 로딩이 추가됨
		const ENetMode OwnerNetMode = GetOwner()->GetNetMode();
		bool bLoadClient = GIsEditor || (OwnerNetMode != NM_DedicatedServer);
		bool bLoadServer = GIsEditor || (OwnerNetMode != NM_Client);

		if (bLoadClient)
		{
			BundlesToLoad.Add(UGameFeaturesSubsystemSettings::LoadStateClient);
		}

		if (bLoadServer)
		{
			BundlesToLoad.Add(UGameFeaturesSubsystemSettings::LoadStateServer);
		}
	}

	FStreamableDelegate OnAssetsLoadedDelegate = FStreamableDelegate::CreateUObject(this, &UMyLyraExperienceManagerComponent::OnExperienceLoadComplete);

	TSharedPtr<FStreamableHandle> Handle = AssetManager.ChangeBundleStateForPrimaryAssets(
		BundleAssetList.Array(),
		BundlesToLoad,
		{}, false, FStreamableDelegate(), FStreamableManager::AsyncLoadHighPriority);

	if (Handle.IsValid() == false || Handle->HasLoadCompleted())
	{
		// 로딩 완료되면, ExecuteDelegate를 통해 OnAssetsLoadedDelegate 호출
		FStreamableHandle::ExecuteDelegate(OnAssetsLoadedDelegate);
	}
	else
	{
		Handle->BindCompleteDelegate(OnAssetsLoadedDelegate);
		Handle->BindCancelDelegate(FStreamableDelegate::CreateLambda([OnAssetsLoadedDelegate]()
		{
			OnAssetsLoadedDelegate.ExecuteIfBound();
		}));
	}

	static int32 StartExperienceLoad_FrameNumber = GFrameNumber;
}

void UMyLyraExperienceManagerComponent::OnExperienceLoadComplete()
{
	static int32 OnExperienceLoadComplete_FrameNumber = GFrameNumber;

	// 해당 함수가 불리는 것은 앞서 보았던 StreamableDelegateDelayHelper에 의해 불림
	OnExperienceFullLoadComplete();
}

void UMyLyraExperienceManagerComponent::OnExperienceFullLoadComplete()
{
	check(LoadState != EMyLyraExperienceLoadState::Loaded);

	LoadState = EMyLyraExperienceLoadState::Loaded;
	OnExperienceLoaded.Broadcast(CurrentExperience);
	OnExperienceLoaded.Clear();
}

const UMyLyraExperienceDefinition* UMyLyraExperienceManagerComponent::GetCurrentExperienceChecked() const
{
	check(LoadState == EMyLyraExperienceLoadState::Loaded);
	check(CurrentExperience != nullptr);
	return CurrentExperience;
}
