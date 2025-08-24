// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "GameplayTagContainer.h"
#include "UIExtensionSystem.h"
#include "GameFeatureAction_WorldActionBase.h"
#include "Components/GameFrameworkComponentManager.h"
#include "GameFeatureAction_AddWidgets.generated.h"

struct FWorldContext;
class UCommonActivatableWidget;

/** HUD의 Layout 요청 */
USTRUCT()
struct FMyLyraHUDLayoutRequest
{
	GENERATED_BODY()

	/** UI의 레이아웃으로 CommonActivatableWidget을 사용 */
	UPROPERTY(EditAnywhere, Category = UI, meta = (AssetBundles = "Client"))
	TSoftClassPtr<UCommonActivatableWidget> LayoutClass;

	/** 앞서 보았던 PrimaryGameLayout의 LayerID를 의미 */
	UPROPERTY(EditAnywhere, Category = UI)
	FGameplayTag LayerID;
};

USTRUCT()
struct FMyLyraHUDElementEntry
{
	GENERATED_BODY()

	/** MyLyraHUDLayout 위에 올릴 대상이 되는 Widget Class */
	UPROPERTY(EditAnywhere, Category = UI, meta = (AssetBundles = "Client"))
	TSoftClassPtr<UUserWidget> WidgetClass;

	/**  SlotID는 MyLyraHUDLayoutRequest에 올린 LayoutClass에 정의된 Slot(GameplayTag)를 의미 */
	UPROPERTY(EditAnywhere, Category = UI)
	FGameplayTag SlotID;
};

/**
 * 
 */
UCLASS()
class MYLYRA_API UGameFeatureAction_AddWidgets : public UGameFeatureAction_WorldActionBase
{
	GENERATED_BODY()

public:
	struct FPerContextData
	{
		TArray<TSharedPtr<FComponentRequestHandle>> ComponentRequests;
		TArray<TWeakObjectPtr<UCommonActivatableWidget>> LayoutsAdded;

		/** Lyra에서 HUDElement는 UIExtension으로 관리 됨 */
		TArray<FUIExtensionHandle> ExtensionHandles;
	};

	void AddWidgets(AActor* Actor, FPerContextData& ActiveData);
	void RemoveWidgets(AActor* Actor, FPerContextData& ActiveData);

	/** UGameFeatureAction's Interface */
	virtual void OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context) override;

	/** UGameFeatureAction_WorldActionBase's Interface */
	virtual void AddToWorld(const FWorldContext& WorldContext, const FGameFeatureStateChangeContext& ChangeContext) override;

	void HandleActorExtension(AActor* Actor, FName EventName, FGameFeatureStateChangeContext ChangeContext);

	/** GFA Add / Remove 상태 관리 */
	TMap<FGameFeatureStateChangeContext, FPerContextData> ContextData;

	/**
	 * GFA_AddWidget은 형태를 정의하는 Layout과 Layout 위에 올릴 Widget 객체로 Widgets으로 구성됨
	 * GameFeatureAction( = GFA)
	 */
	UPROPERTY(EditAnywhere, Category = UI)
	TArray<FMyLyraHUDLayoutRequest> Layout;

	UPROPERTY(EditAnywhere, Category = UI)
	TArray<FMyLyraHUDElementEntry> Widgets;
};
