// Fill out your copyright notice in the Description page of Project Settings.

#include "MyLyraPawnExtensionComponent.h"
#include "Components/GameFrameworkComponentManager.h"
#include "MyLyra/MyLyraGameplayTags.h"
#include "MyLyra/MyLyraLogChannels.h"
#include "MyLyra/Character/MyLyraPawnData.h"

const FName UMyLyraPawnExtensionComponent::NAME_ActorFeatureName("PawnExtension");

UMyLyraPawnExtensionComponent::UMyLyraPawnExtensionComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;
}

void UMyLyraPawnExtensionComponent::SetPawnData(const UMyLyraPawnData* InPawnData)
{
	// Pawn에 대해 Authority가 없을 경우, SetPawnData 진행 중단
	APawn* Pawn = GetPawnChecked<APawn>();
	if (Pawn->GetLocalRole() != ROLE_Authority)
	{
		return;
	}

	if (IsValid(PawnData))
	{
		return;
	}

	// PawnData Update!
	PawnData = InPawnData;
}

void UMyLyraPawnExtensionComponent::SetupPlayerInputComponent()
{
	// ForceUpdate로 다시 InitState 상태 변환 시작(연결 고리)
	CheckDefaultInitialization();
}

PRAGMA_DISABLE_OPTIMIZATION

void UMyLyraPawnExtensionComponent::OnRegister()
{
	Super::OnRegister();

	// 정상적인 Actor에 등록되어 있는지를 확인
	{
		if (GetPawn<APawn>() == nullptr)
		{
			UE_LOG(LogMyLyra, Error, TEXT("This component has been added to a BP whose base class is not a Pawn!"));
			return;
		}
	}

	// GameFrameworkComponentManager에 InitState 사용을 위해 등록 진행
	// - 등록은 상속 받았던 IGameFrameworkInitStateInterface 함수 RegisterInitStateFeature() 활용
	RegisterInitStateFeature();

	// 디버깅을 위한 함수
	UGameFrameworkComponentManager* Manager = UGameFrameworkComponentManager::GetForActor(GetOwningActor());
}
PRAGMA_ENABLE_OPTIMIZATION

void UMyLyraPawnExtensionComponent::BeginPlay()
{
	Super::BeginPlay();

	// FeatureName에 NAME_None을 넣으면, Actor에 등록된 Feature Component의 InitState 상태를 관찰하겠다는 의미.
	// FeatureName에 특정 이름을 넣으면, Actor의 등록된 해당되는 이름의 Feature Component의 InitState 상태를 관찰하겠다는 것.
	BindOnActorInitStateChanged(NAME_None, FGameplayTag::EmptyTag, false);

	// InitState_Spawned 로 상태 변환
	// - TryToChangeInitState 는 아래와 같이 진행
	// 1. CanChangeInitState 로 상태 변환 가능성 유무 판단
	// 2. HandleChangeInitState 로 내부 상태 변경 (Feature Component)
	// 3. BindOnActorInitStateChanged 로 Bind된 Delegate를 조건에 맞게 호출해 줌
	//		- MyLyraPwanExtensionComponent의 경우, 모든 Actor의 Feature 상태 변화에 대해 OnActorInitStateChanged()가 호출됨
	//
	// TryToChangeInitState : InitState 상태를 입력한 매개변수 상태로 업데이트 하는 것을 시도하겠다는 것.
	ensure(TryToChangeInitState(FMyLyraGameplayTags::Get().InitState_Spawned));

	// 해당 함수는 Overriding 한다.
	// - 이 함수를 ForceUpdateInitState와 같은 느낌으로 이해하면 좋음
	// - 현재 강제 업데이트 진행 (CancChangInitState 와 HandleChangeInitState를 진행해 줌)
	CheckDefaultInitialization();
}

void UMyLyraPawnExtensionComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// 앞서, OnRegister의 RegisterInitStateFeature()의 쌍을 맞춰주자 
	UnregisterInitStateFeature();

	Super::EndPlay(EndPlayReason);
}

FName UMyLyraPawnExtensionComponent::GetFeatureName() const
{
	return NAME_ActorFeatureName;
}

void UMyLyraPawnExtensionComponent::OnActorInitStateChanged(const FActorInitStateChangedParams& Params)
{
	if (Params.FeatureName != NAME_ActorFeatureName)	// 나 자신은 제외함을 의미
	{
		// MyLyraPawnExtensionComponent는 다른 Feature Component들의 상태가 DataAvailable를 관찰하여, Sync를 맞추는 구간이 있었다. (CanChangeInitState)
		// - 이를 가능케하기 위해, OnActorInitStateChanged에선 DataAvailable에 대해 지속적으로 CheckDefaultInitialization을 호출해, 상태를 확인한다.
		const FMyLyraGameplayTags& InitTags = FMyLyraGameplayTags::Get();
		if (Params.FeatureState == InitTags.InitState_DataAvailable)
		{
			CheckDefaultInitialization();
		}
	}
}

bool UMyLyraPawnExtensionComponent::CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const
{
	check(Manager);

	APawn* Pawn = GetPawn<APawn>();
	const FMyLyraGameplayTags& InitTags = FMyLyraGameplayTags::Get();

	// InitState_Spawned 초기화
	if (CurrentState.IsValid() == false && DesiredState == InitTags.InitState_Spawned)
	{
		// Pawn이 잘 세팅만 되어 있으면 바로 Spawned 로 넘어감
		if (IsValid(Pawn))
		{
			return true;
		}
	}

	// Spawned -> DataAvailable
	if (CurrentState == InitTags.InitState_Spawned && DesiredState == InitTags.InitState_DataAvailable)
	{
		// 아마 PawnData를 누군가 설정하는 상태
		if (IsValid(PawnData) == false)
		{
			return false;
		}

		// LocallyControlled인 Pawn이 Controller가 없으면 Error
		const bool bIsLocallyControlled = Pawn->IsLocallyControlled();
		if (bIsLocallyControlled)
		{
			if (GetController<AController>() == nullptr)
			{
				return false;
			}
		}

		return true;
	}

	// DataAvailable -> DataInitialized
	if (CurrentState == InitTags.InitState_DataAvailable && DesiredState == InitTags.InitState_DataInitialized)
	{
		// Actor 에 바인드된 모든 Feature들이 DataAvailable 상태일 때, DataInitialized 로 넘어감
		// - HaveAllFeaturesReachedInitState 확인
		return Manager->HaveAllFeaturesReachedInitState(Pawn, InitTags.InitState_DataAvailable);
	}

	// DataInitialized -> GameplayReady
	if (CurrentState == InitTags.InitState_DataInitialized && DesiredState == InitTags.InitState_GameplayReady)
	{
		return true;
	}

	// 위의 선형적인(Linear) transition이 아니면 false
	return false;
}

void UMyLyraPawnExtensionComponent::CheckDefaultInitialization()
{
	// PawnExtensionComponent는 Feature Component들의 초기화를 관장하는 Component
	// - 따라서, Actor에 바인딩된 Feature Component들에 대해 CheckDefaultInitialization을 호출해주도록 한다. (ForceUpdate 느낌 ?)
	// - 이 함수를 IGameFrameworkInitStateInterface가 제공하는데, CheckDefaultInitializationForImplementers 이다.
	CheckDefaultInitializationForImplementers();

	const FMyLyraGameplayTags& InitTags = FMyLyraGameplayTags::Get();

	// 사용자 정의 InitState를 직적 넘겨줘야 한다. (번거롭다.)
	static const TArray<FGameplayTag> StateChain = {InitTags.InitState_Spawned, InitTags.InitState_DataAvailable, InitTags.InitState_DataInitialized, InitTags.InitState_GameplayReady};

	// CanChangeInitState() 와 HandleChangeInitState() 그리고 ChangeFeatureInitState 호출을 통한 OnActorInitStateChanged Delegate 호출까지 진행해 줌
	// - 상태 업데이트가 가능할 때까지 지속한다.
	// - InitState에 대한 변화는 Linear(선형적)
	// - 업데이트가 멈추면 누군가 시작해야 함을 의미 (chaining)
	ContinueInitStateChain(StateChain);
}
