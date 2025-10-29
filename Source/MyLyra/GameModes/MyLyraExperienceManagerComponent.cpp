// Fill out your copyright notice in the Description page of Project Settings.

#include "MyLyraExperienceManagerComponent.h"
#include "GameFeatureAction.h"
#include "GameFeaturesSubsystem.h"
#include "MyLyraExperienceDefinition.h"
#include "MyLyra/System/MyLyraAssetManager.h"
#include "GameFeaturesSubsystemSettings.h"
#include "MyLyraExperienceActionSet.h"

UMyLyraExperienceManagerComponent::UMyLyraExperienceManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

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

PRAGMA_DISABLE_OPTIMIZATION
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

	// 게임 플레이에 필요한 Experience를 로딩함
	StartExperienceLoad();
}
PRAGMA_ENABLE_OPTIMIZATION

PRAGMA_DISABLE_OPTIMIZATION
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

	// ExperienceActionSet 순회하며, BundleAssetList로 추가
	for (const TObjectPtr<UMyLyraExperienceActionSet>& ActionSet : CurrentExperience->ActionSets)
	{
		if (ActionSet)
		{
			// 앞서 생성한 HAS_Shooter_SharedHUD가 추가됨
			// - BundleAssetList는 Bundle로 등록할 Root의 PrimaryDataAsset을 추가하는 과정 (ChangeBundleStateForPrimaryAssets 참고)
			BundleAssetList.Add(ActionSet->GetPrimaryAssetId());
		}
	}
	
	/**
	 * Load Assets associated with the experience
	 * 후일 GameFeature를 사용해, Experience에 바인됭된 GameFeature Plugin을 로딩할 Bundle 이름을 추가
	 * - Bundle 이라는 것은 나중에 로딩할 Asset의 카테고리 이름이라 생각하면 됨
	 */
	TArray<FName> BundlesToLoad;
	{
		// OwnerNetMode 가 NM_Standalone이면 ? Client / Server 둘 다 로딩이 추가됨
		const ENetMode OwnerNetMode = GetOwner()->GetNetMode();
		const bool bLoadClient = GIsEditor || (OwnerNetMode != NM_DedicatedServer);
		const bool bLoadServer = GIsEditor || (OwnerNetMode != NM_Client);

		if (bLoadClient)
		{
			BundlesToLoad.Add(UGameFeaturesSubsystemSettings::LoadStateClient);
		}

		if (bLoadServer)
		{
			BundlesToLoad.Add(UGameFeaturesSubsystemSettings::LoadStateServer);
		}
	}

	FStreamableDelegate OnAssetsLoadedDelegate = FStreamableDelegate::CreateUObject(this, &ThisClass::OnExperienceLoadComplete);

	TSharedPtr<FStreamableHandle> Handle = AssetManager.ChangeBundleStateForPrimaryAssets(
		BundleAssetList.Array(),
		BundlesToLoad,
		{}, false, FStreamableDelegate(), FStreamableManager::AsyncLoadHighPriority);

	if (!Handle.IsValid() || Handle->HasLoadCompleted())
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
PRAGMA_ENABLE_OPTIMIZATION

// Experience가 로딩되면, Callback으로 호출되는 함수
void UMyLyraExperienceManagerComponent::OnExperienceLoadComplete()
{
	static int32 OnExperienceLoadComplete_FrameNumber = GFrameNumber;

	check(LoadState == EMyLyraExperienceLoadState::Loading);
	check(CurrentExperience);

	// 이전 활성화된 GameFeature Plugin의 URL 클리어
	GameFeaturePluginURLs.Reset();

	// std::function<void(const UPrimaryDataAsset*, const TArray<FString>&)>
	// TFunction<void(const UPrimaryDataAsset*, const TArray<FString>&)> CollectGameFeaturePluginURLs = [This = this](const UPrimaryDataAsset* Context, const TArray<FString>& FeaturePluginList)
	auto CollectGameFeaturePluginURLs = [This = this](const UPrimaryDataAsset* Context, const TArray<FString>& FeaturePluginList)
	{
		// FeaturePluginList를 순회하며, PluginURL을 ExperienceManagerComponent의 GameFeaturePluginURLs에 추가
		for (const FString& PluginName : FeaturePluginList)
		{
			FString PluginURL;
			if (UGameFeaturesSubsystem::Get().GetPluginURLByName(PluginName, PluginURL))
			{
				This->GameFeaturePluginURLs.AddUnique(PluginURL);
			}
		}
	};

	// GameFeaturesToEnable에 있는 Plugin만 일단 활성화할 GameFeature Plugin 후보군으로 등록
	CollectGameFeaturePluginURLs(CurrentExperience, CurrentExperience->GameFeaturesToEnable);

	// GameFeaturePluginURLs에 등록된 Plugin을 로딩 및 활성화
	NumGameFeaturePluginsLoading = GameFeaturePluginURLs.Num();
	if (NumGameFeaturePluginsLoading)
	{
		LoadState = EMyLyraExperienceLoadState::LoadingGameFeatures;
		for (const FString& PluginURL : GameFeaturePluginURLs)
		{
			// 매 Plugin이 로딩 및 활성화 이후, OnGameFeaturePluginLoadComplete 롤백 함수 등록
			UGameFeaturesSubsystem::Get().LoadAndActivateGameFeaturePlugin(PluginURL, FGameFeaturePluginLoadComplete::CreateUObject(this, &ThisClass::OnGameFeaturePluginLoadComplete));
		}
	}
	else
	{
		// 해당 함수가 불리는 것은 앞서 보았던 StreamableDelegateDelayHelper에 의해 불림
		OnExperienceFullLoadComplete();
	}
}

void UMyLyraExperienceManagerComponent::OnGameFeaturePluginLoadComplete(const UE::GameFeatures::FResult& InResult)
{
	// 매 GameFeature Plugin이 로딩될 때, 해당 함수가 롤백으로 불림
	NumGameFeaturePluginsLoading--;
	if (NumGameFeaturePluginsLoading == 0)
	{
		// GameFeaturePlugin 로딩이 다 끝났을 경우, 기존대로 Loaded로서, OnExperienceFullLoadCompleted 호출
		// GameFeaturePlugin 로딩과 활성화가 끝났다면? UGameFeatureAction을 활성화 해야하나?
		OnExperienceFullLoadComplete();
	}
}

PRAGMA_DISABLE_OPTIMIZATION
void UMyLyraExperienceManagerComponent::OnExperienceFullLoadComplete()
{
	check(LoadState != EMyLyraExperienceLoadState::Loaded)

	// GameFeature Plugin 의 로딩과 활성화 이후, GameFeature Action들을 활성화
	{
		LoadState = EMyLyraExperienceLoadState::ExecutingActions;

		// GameFeatureAction 활성화를 위한 Context 준비
		FGameFeatureActivatingContext Context;
		{
			// World의 handle을 세팅
			const FWorldContext* ExistingWorldContext = GEngine->GetWorldContextFromWorld(GetWorld());
			if (ExistingWorldContext)
			{
				Context.SetRequiredWorldContextHandle(ExistingWorldContext->ContextHandle);
			}
		}

		// TFunction<void(const TArray<UGameFeatureAction*>&)> ActivateListOfActions = [&Context](const TArray<UGameFeatureAction*>& InActionList)
		auto ActivateListOfActions = [&Context](const TArray<UGameFeatureAction*>& ActionList)
		{
			for (UGameFeatureAction* Action : ActionList)
			{
				// 명시적으로 GameFeatureAction에 대해 Registering -> Loading -> Activating 순으로 호출
				if (Action)
				{
					Action->OnGameFeatureRegistering();
					Action->OnGameFeatureLoading();
					Action->OnGameFeatureActivating(Context);
				}
			}
		};

		// 1. Experience의 Actions
		ActivateListOfActions(CurrentExperience->Actions);

		// 2. Experience의 ActionSets
		for (const TObjectPtr<UMyLyraExperienceActionSet>& ActionSet : CurrentExperience->ActionSets)
		{
			ActivateListOfActions(ActionSet->Actions);
		}
	}

	LoadState = EMyLyraExperienceLoadState::Loaded;
	OnExperienceLoaded.Broadcast(CurrentExperience);
	OnExperienceLoaded.Clear();
}
PRAGMA_ENABLE_OPTIMIZATION

const UMyLyraExperienceDefinition* UMyLyraExperienceManagerComponent::GetCurrentExperienceChecked() const
{
	check(LoadState == EMyLyraExperienceLoadState::Loaded);
	check(CurrentExperience != nullptr);
	return CurrentExperience;
}
