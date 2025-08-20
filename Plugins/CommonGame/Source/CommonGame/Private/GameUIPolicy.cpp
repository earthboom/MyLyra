// Fill out your copyright notice in the Description page of Project Settings.

#include "GameUIPolicy.h"
#include "CommonLocalPlayer.h"
#include "PrimaryGameLayout.h"

UPrimaryGameLayout* UGameUIPolicy::GetRootLayout(const UCommonLocalPlayer* LocalPlayer) const
{
	return nullptr;
}

TSubclassOf<UPrimaryGameLayout> UGameUIPolicy::GetLayoutWidgetClass(UCommonLocalPlayer* LocalPlayer)
{
	return LayoutClass.LoadSynchronous();
}

void UGameUIPolicy::CreateLayoutWidget(UCommonLocalPlayer* LocalPlayer)
{
	// PlayerController가 할당되었을 경우, LayoutWidget을 생성
	APlayerController* PC = LocalPlayer->GetPlayerController(GetWorld());
	if (IsValid(PC))
	{
		// LayoutWidgetClass가 있을 경우
		TSubclassOf<UPrimaryGameLayout> LayoutWidgetClass = GetLayoutWidgetClass(LocalPlayer);
		if (ensure(IsValid(LayoutWidgetClass) && LayoutWidgetClass->HasAnyClassFlags(CLASS_Abstract) == false))
		{
			// PlayerController가 소유한다는 의미에서 Owner 설정
			UPrimaryGameLayout* NewLayoutObject = CreateWidget<UPrimaryGameLayout>(PC, LayoutWidgetClass);

			RootViewportLayouts.Emplace(LocalPlayer, NewLayoutObject, true);

			AddLayoutToViewport(LocalPlayer, NewLayoutObject);
		}
	}
}

void UGameUIPolicy::AddLayoutToViewport(UCommonLocalPlayer* LocalPlayer, UPrimaryGameLayout* Layout)
{
	// CommonUserWidget에 PlayerContext를 설정
	Layout->SetPlayerContext(FLocalPlayerContext(LocalPlayer));

	// 해당 Layout에 우선순위를 높게 설정
	// - AddToPlayerScreen을 통해 Widget을 붙임
	Layout->AddToPlayerScreen(1000);
}

void UGameUIPolicy::RemoveLayoutFromViewport(UCommonLocalPlayer* LocalPlayer, UPrimaryGameLayout* Layout)
{
	// UCommonUserWidget의 SlateWidget을 가져와 Parent와 연결고리를 끊는다.
	TWeakPtr<SWidget> LayoutSlateWidget = Layout->GetCachedWidget();
	if (LayoutSlateWidget.IsValid())
	{
		Layout->RemoveFromParent();
	}
}

void UGameUIPolicy::NotifyPlayerAdded(UCommonLocalPlayer* LocalPlayer)
{
	// PlayerController가 업데이트되면, GameLayout을 업데이트 하기 위해 Delegate 추가
	LocalPlayer->OnPlayerControllerSet.AddWeakLambda(this, [this](UCommonLocalPlayer* LocalPlayer, APlayerController* PlayerController)
	{
		// 우선 추가된 Player가 있으면, 제거
		NotifyPlayerRemoved(LocalPlayer);

		// RootViewportLayouts를 순회하며 검색
		// - FRootViewportLayoutInfo의 operator== 정의
		FRootViewportLayoutInfo* LayoutInfo = RootViewportLayouts.FindByKey(LocalPlayer);
		if (LayoutInfo)
		{
			// Layout만 업데이트
			AddLayoutToViewport(LocalPlayer, LayoutInfo->RootLayout);
			LayoutInfo->bAddedToViewport = true;
		}
		else
		{
			// Layout 생성하고 활성화
			CreateLayoutWidget(LocalPlayer);
		}
	});

	FRootViewportLayoutInfo* LayoutInfo = RootViewportLayouts.FindByKey(LocalPlayer);
	if (LayoutInfo)
	{
		AddLayoutToViewport(LocalPlayer, LayoutInfo->RootLayout);
		LayoutInfo->bAddedToViewport = true;
	}
	else
	{
		CreateLayoutWidget(LocalPlayer);
	}
}

void UGameUIPolicy::NotifyPlayerRemoved(UCommonLocalPlayer* LocalPlayer)
{
	FRootViewportLayoutInfo* LayoutInfo = RootViewportLayouts.FindByKey(LocalPlayer);
	if (LayoutInfo)
	{
		RemoveLayoutFromViewport(LocalPlayer, LayoutInfo->RootLayout);

		// 비활성화 확인
		LayoutInfo->bAddedToViewport = false;
	}
}

void UGameUIPolicy::NotifyPlayerDestroyed(UCommonLocalPlayer* LocalPlayer)
{
	NotifyPlayerRemoved(LocalPlayer);

	// Player가 Destroy 되므로, OnPlayerControllerSet에서 제거
	LocalPlayer->OnPlayerControllerSet.RemoveAll(this);

	// RootViewportLayouts에서 제거
	const int32 LayoutInfoIdx = RootViewportLayouts.IndexOfByKey(LocalPlayer);
	if (LayoutInfoIdx != INDEX_NONE)
	{
		// PrimaryGameLayout이 있으면, Viewport에서도 제거됨
		UPrimaryGameLayout* Layout = RootViewportLayouts[LayoutInfoIdx].RootLayout;
		RootViewportLayouts.RemoveAt(LayoutInfoIdx);
		RemoveLayoutFromViewport(LocalPlayer, Layout);
	}
}
