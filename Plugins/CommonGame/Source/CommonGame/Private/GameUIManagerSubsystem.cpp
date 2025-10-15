// Fill out your copyright notice in the Description page of Project Settings.

#include "GameUIManagerSubsystem.h"
#include "GameUIPolicy.h"

UGameUIManagerSubsystem::UGameUIManagerSubsystem()
	: Super()
{
}

void UGameUIManagerSubsystem::SwitchToPolicy(UGameUIPolicy* InPolicy)
{
	if (CurrentPolicy != InPolicy)
	{
		CurrentPolicy = InPolicy;
	}
}

void UGameUIManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// CurrentPolicy가 설정이 안되어 있고 DefaultUIPolicyClass가 제대로 설정되어 있을 경우
	if (IsValid(CurrentPolicy) == false && DefaultUIPolicyClass.IsNull() == false)
	{
		// UIPolicyClass는 BP 에셋 로드
		TSubclassOf<UGameUIPolicy> PolicyClass = DefaultUIPolicyClass.LoadSynchronous();

		// UIPolicyClass를 통해 NewObject로 인스턴싱
		SwitchToPolicy(NewObject<UGameUIPolicy>(this, PolicyClass));
	}
}

void UGameUIManagerSubsystem::Deinitialize()
{
	Super::Deinitialize();

	SwitchToPolicy(nullptr);
}

// false를 반환하게 되면 이 Subsystem은 생성되지 않음
bool UGameUIManagerSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	// DedicatedServer의 경우, GameUIManagerSubsystem을 활성화하지 않음
	if (CastChecked<UGameInstance>(Outer)->IsDedicatedServerInstance() == false)
	{
		// 만약 어떠한 Subsystem도 UGameUIManagerSubsystem을 상속 받지 않았다면, 해당 GameInstanceSubsystem은 활성화되지 않음
		TArray<UClass*> ChildClasses;
		GetDerivedClasses(GetClass(), ChildClasses, false);

		return ChildClasses.Num() == 0;
	}

	return false;
}

void UGameUIManagerSubsystem::NotifyPlayerAdded(UCommonLocalPlayer* LocalPlayer)
{
	if (ensure(LocalPlayer) && CurrentPolicy)
	{
		CurrentPolicy->NotifyPlayerAdded(LocalPlayer);
	}
}

void UGameUIManagerSubsystem::NotifyPlayerRemoved(UCommonLocalPlayer* LocalPlayer)
{
	if (LocalPlayer && CurrentPolicy)
	{
		CurrentPolicy->NotifyPlayerRemoved(LocalPlayer);
	}
}

void UGameUIManagerSubsystem::NotifyPlayerDestroyed(UCommonLocalPlayer* LocalPlayer)
{
	if (LocalPlayer && CurrentPolicy)
	{
		CurrentPolicy->NotifyPlayerDestroyed(LocalPlayer);
	}
}
