// Fill out your copyright notice in the Description page of Project Settings.

#include "MyLyraPawnExtensionComponent.h"
#include "Components/GameFrameworkComponentManager.h"
#include "MyLyra/MyLyraGameplayTags.h"
#include "MyLyra/MyLyraLogChannels.h"

const FName UMyLyraPawnExtensionComponent::NAME_ActorFeatureName("PawnExtension");

UMyLyraPawnExtensionComponent::UMyLyraPawnExtensionComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;
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
	IGameFrameworkInitStateInterface::OnActorInitStateChanged(Params);
}

bool UMyLyraPawnExtensionComponent::CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const
{
	return IGameFrameworkInitStateInterface::CanChangeInitState(Manager, CurrentState, DesiredState);
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
