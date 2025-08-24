// Fill out your copyright notice in the Description page of Project Settings.

#include "GameFeatureAction_AddWidgets.h"
#include "CommonActivatableWidget.h"
#include "CommonUIExtensions.h"
#include "Components/GameFrameworkComponentManager.h"
#include "MyLyra/UI/MyLyraHUD.h"

void UGameFeatureAction_AddWidgets::AddWidgets(AActor* Actor, FPerContextData& ActiveData)
{
	AMyLyraHUD* HUD = CastChecked<AMyLyraHUD>(Actor);

	// HUD를 통해, LocalPlayer를 가져오자
	ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(HUD->GetOwningPlayerController()->Player);
	if (IsValid(LocalPlayer))
	{
		// Layout의 요청 순회
		for (const FMyLyraHUDLayoutRequest& Entry : Layout)
		{
			TSubclassOf<UCommonActivatableWidget> ConcreteWidgetClass = Entry.LayoutClass.Get();
			if (IsValid(ConcreteWidgetClass))
			{
				ActiveData.LayoutsAdded.Add(UCommonUIExtensions::PushContentToLayer_ForPlayer(LocalPlayer, Entry.LayerID, ConcreteWidgetClass));
			}
		}

		// Widget을 순회하며, UIExtensionSubsystem의 Extension에 추가
		UUIExtensionSubsystem* ExtensionSubsystem = HUD->GetWorld()->GetSubsystem<UUIExtensionSubsystem>();
		for (const FMyLyraHUDElementEntry& Entry : Widgets)
		{
			ActiveData.ExtensionHandles.Add(ExtensionSubsystem->RegisterExtensionAsWidgetForContext(Entry.SlotID, LocalPlayer, Entry.WidgetClass.Get(), -1));
		}
	}
}

void UGameFeatureAction_AddWidgets::RemoveWidgets(AActor* Actor, FPerContextData& ActiveData)
{
	AMyLyraHUD* HUD = CastChecked<AMyLyraHUD>(Actor);

	// MyLyraHUD에 추가된 CommonActivatableWidget을 순회, Deactivate 시켜줌
	for (TWeakObjectPtr<UCommonActivatableWidget>& AddedLayout : ActiveData.LayoutsAdded)
	{
		if (AddedLayout.IsValid())
		{
			AddedLayout->DeactivateWidget();
		}
	}
	ActiveData.LayoutsAdded.Reset();

	// UIExtension에 대해 순회, Unregister
	for (FUIExtensionHandle& Handle : ActiveData.ExtensionHandles)
	{
		// Unregister는 UUIExtensionSystem에서 제거가 올바르게 되어야 함
		Handle.Unregister();
	}
	ActiveData.ExtensionHandles.Reset();
}

void UGameFeatureAction_AddWidgets::OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context)
{
	Super::OnGameFeatureDeactivating(Context);
}

void UGameFeatureAction_AddWidgets::AddToWorld(const FWorldContext& WorldContext, const FGameFeatureStateChangeContext& ChangeContext)
{
	UWorld* World = WorldContext.World();
	UGameInstance* GameInstance = WorldContext.OwningGameInstance;
	FPerContextData& ActiveData = ContextData.FindOrAdd(ChangeContext);

	// GameFrameworkComponentManager를 가져올 GameInstance와 World는 GameWorld여야 함
	if (IsValid(GameInstance) && IsValid(World) && World->IsGameWorld())
	{
		// GameFrameworkComponentManager 가져옴
		UGameFrameworkComponentManager* ComponentManager = UGameInstance::GetSubsystem<UGameFrameworkComponentManager>(GameInstance);
		if (IsValid(ComponentManager))
		{
			// 기본적으로 Widget을 추가할 대상으로 AMyLyraHUD로 고정
			TSoftClassPtr<AActor> HUDActorClass = AMyLyraHUD::StaticClass();

			// GFA_WorldBase와 마찬가지로 HandleActorExtension을 콜백으로 받도록 함
			TSharedPtr<FComponentRequestHandle> ExtensionRequestHandle = ComponentManager->AddExtensionHandler(
				HUDActorClass,
				UGameFrameworkComponentManager::FExtensionHandlerDelegate::CreateUObject(this, &ThisClass::HandleActorExtension, ChangeContext));

			ActiveData.ComponentRequests.Add(ExtensionRequestHandle);
		}
	}
}

void UGameFeatureAction_AddWidgets::HandleActorExtension(AActor* Actor, FName EventName, FGameFeatureStateChangeContext ChangeContext)
{
	FPerContextData& ActiveData = ContextData.FindOrAdd(ChangeContext);

	// Receiver인 AMyLyraHUD의 Removed/Added에 따라 Widget을 추가/제거
	if ((EventName == UGameFrameworkComponentManager::NAME_ExtensionRemoved) ||
		(EventName == UGameFrameworkComponentManager::NAME_ReceiverRemoved))
	{
		RemoveWidgets(Actor, ActiveData);
	}
	else if ((EventName == UGameFrameworkComponentManager::NAME_ExtensionAdded) ||
		(EventName == UGameFrameworkComponentManager::NAME_GameActorReady))
	{
		AddWidgets(Actor, ActiveData);
	}
}
