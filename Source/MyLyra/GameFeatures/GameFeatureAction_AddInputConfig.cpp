// Fill out your copyright notice in the Description page of Project Settings.

#include "GameFeatureAction_AddInputConfig.h"
#include "EnhancedInputSubsystems.h"
#include "PlayerMappableInputConfig.h"
#include "Components/GameFrameworkComponentManager.h"
#include "MyLyra/Character/MyLyraHeroComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GameFeatureAction_AddInputConfig)

void UGameFeatureAction_AddInputConfig::OnGameFeatureActivating(FGameFeatureActivatingContext& Context)
{
	FPerContextData& ActiveData = ContextData.FindOrAdd(Context);
	if (ensure(ActiveData.ExtensionRequestHandles.IsEmpty()) == false || ensure(ActiveData.PawnsAddedTo.IsEmpty()) == false)
	{
		Reset(ActiveData);
	}

	// UGameFeatureAction_WorldActionbase를 호출하며, AddToWorld() 호출
	Super::OnGameFeatureActivating(Context);
}

void UGameFeatureAction_AddInputConfig::OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context)
{
	Super::OnGameFeatureDeactivating(Context);

	FPerContextData* ActiveData = ContextData.Find(Context);
	if (ensure(ActiveData))
	{
		Reset(*ActiveData);
	}
}

void UGameFeatureAction_AddInputConfig::AddToWorld(const FWorldContext& WorldContext, const FGameFeatureStateChangeContext& ChangeContext)
{
	UWorld* World = WorldContext.World();
	UGameInstance* GameInstance = WorldContext.OwningGameInstance;
	FPerContextData& ActiveData = ContextData.FindOrAdd(ChangeContext);

	if (IsValid(GameInstance) && IsValid(World) && World->IsGameWorld())
	{
		// GameFrameworkComponentManager을 이용해, ExtensionHandler를 추가해 등록 진행
		// - HandlePawnExtension 콜백함수로 연결
		UGameFrameworkComponentManager* ComponentManager = UGameInstance::GetSubsystem<UGameFrameworkComponentManager>(GameInstance);
		if (IsValid(ComponentManager))
		{
			UGameFrameworkComponentManager::FExtensionHandlerDelegate AddConfigDelegate = UGameFrameworkComponentManager::FExtensionHandlerDelegate::CreateUObject(this, &UGameFeatureAction_AddInputConfig::HandlePawnExtension, ChangeContext);

			// 등록된 콜백 함수의 핸들을 ActiveDta의 ExtensionRequestHandles 에 등록
			TSharedPtr<FComponentRequestHandle> ExtensionRequestHandle = ComponentManager->AddExtensionHandler(APawn::StaticClass(), AddConfigDelegate);
			ActiveData.ExtensionRequestHandles.Add(ExtensionRequestHandle);
		}
	}
}

void UGameFeatureAction_AddInputConfig::HandlePawnExtension(AActor* Actor, FName EventName, FGameFeatureStateChangeContext ChangeContext)
{
	APawn* AsPawn = CastChecked<APawn>(Actor);
	FPerContextData& ActiveData = ContextData.FindOrAdd(ChangeContext);

	if (EventName == UGameFrameworkComponentManager::NAME_ExtensionAdded || EventName == UMyLyraHeroComponent::NAME_BindInputsNow)
	{
		AddInputConfig(AsPawn, ActiveData);
	}
	else if (EventName == UGameFrameworkComponentManager::NAME_ExtensionRemoved)
	{
		RemoveInputConfig(AsPawn, ActiveData);
	}
}

void UGameFeatureAction_AddInputConfig::AddInputConfig(APawn* Pawn, FPerContextData& ActiveData)
{
	APlayerController* PC = Cast<APlayerController>(Pawn->GetController());
	ULocalPlayer* LP = PC ? PC->GetLocalPlayer() : nullptr;
	if (IsValid(LP))
	{
		UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
		if (IsValid(Subsystem))
		{
			// 추가된 InputConfigs를 순회하며, EnhancedInputSubsystem에 PlayerMappableConfig를 직접 추가
			for (const FMyLyraMappableConfigPair& Pair : InputConfigs)
			{
				FModifyContextOptions Options = {};
				Options.bIgnoreAllPressedKeysUntilRelease = false;

				const UPlayerMappableInputConfig* Config = Pair.Config.LoadSynchronous();
				for (const TPair<TObjectPtr<UInputMappingContext>, int>& MappingContextPair : Config->GetMappingContexts())
				{
					const UInputMappingContext* MappingContext = MappingContextPair.Key;
					const int32 Priority = MappingContextPair.Value;
					Subsystem->AddMappingContext(MappingContext, Priority, Options);
				}
			}

			// ActivateData에 Pawn을 관리대상으로 등록
			ActiveData.PawnsAddedTo.AddUnique(Pawn);
		}
	}
}

void UGameFeatureAction_AddInputConfig::RemoveInputConfig(APawn* Pawn, FPerContextData& ActiveData)
{
	APlayerController* PC = Cast<APlayerController>(Pawn->GetController());
	ULocalPlayer* LP = PC ? PC->GetLocalPlayer() : nullptr;
	if (IsValid(LP))
	{
		UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
		if (IsValid(Subsystem))
		{
			for (const FMyLyraMappableConfigPair& Pair : InputConfigs)
			{
				if (Pair.bShouldActivateAutomatically)
				{
					FModifyContextOptions Options = {};
					Options.bIgnoreAllPressedKeysUntilRelease = false;

					const UPlayerMappableInputConfig* Config = Pair.Config.LoadSynchronous();
					for (const TPair<TObjectPtr<UInputMappingContext>, int>& MappingContextPair : Config->GetMappingContexts())
					{
						const UInputMappingContext* MappingContext = MappingContextPair.Key;
						Subsystem->RemoveMappingContext(MappingContext, Options);
					}
				}
			}

			ActiveData.PawnsAddedTo.Remove(Pawn);
		}
	}
}

void UGameFeatureAction_AddInputConfig::Reset(FPerContextData& ActiveData)
{
	// ExtensionRequestHandles 초기화
	ActiveData.ExtensionRequestHandles.Empty();

	// PawnsAddedTo에 대해, 하나씩 Stack 방식으로 위에서 아래로 직접 InputConfig 제거 진행
	while (ActiveData.PawnsAddedTo.IsEmpty() == false)
	{
		TWeakObjectPtr<APawn> PawnPtr = ActiveData.PawnsAddedTo.Top();
		if (PawnPtr.IsValid())
		{
			RemoveInputConfig(PawnPtr.Get(), ActiveData);
		}
		else
		{
			// WeakObjectPtr로 PawnAddedTo를 관리하고 있기 때문에, GC되었다면, nullptr일 수 있음
			ActiveData.PawnsAddedTo.Pop();
		}
	}
}
