// Fill out your copyright notice in the Description page of Project Settings.

#include "MyLyraGameInstance.h"
#include "Components/GameFrameworkComponentManager.h"
#include "MyLyra/MyLyraGameplayTags.h"

void UMyLyraGameInstance::Init()
{
	Super::Init();

	UGameFrameworkComponentManager* ComponentManager = GetSubsystem<UGameFrameworkComponentManager>(this);
	if (ensure(ComponentManager))
	{
		const FMyLyraGameplayTags& GameplayTags = FMyLyraGameplayTags::Get();

		// 공식문서에서 RegisterInitState는 GameInstance initializezation에서 수행해라고 명시되어 있음
		ComponentManager->RegisterInitState(GameplayTags.InitState_Spawned, false, FGameplayTag());
		ComponentManager->RegisterInitState(GameplayTags.InitState_DataAvailable, false, GameplayTags.InitState_Spawned);
		ComponentManager->RegisterInitState(GameplayTags.InitState_DataInitialized, false, GameplayTags.InitState_Spawned);
		ComponentManager->RegisterInitState(GameplayTags.InitState_GameplayReady, false, GameplayTags.InitState_DataInitialized);
	}
}

void UMyLyraGameInstance::Shutdown()
{
	Super::Shutdown();
}
