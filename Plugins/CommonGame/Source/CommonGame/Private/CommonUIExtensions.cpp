// Fill out your copyright notice in the Description page of Project Settings.

#include "CommonUIExtensions.h"
#include "CommonActivatableWidget.h"
#include "CommonLocalPlayer.h"
#include "GameUIManagerSubsystem.h"
#include "GameUIPolicy.h"
#include "PrimaryGameLayout.h"

UCommonActivatableWidget* UCommonUIExtensions::PushContentToLayer_ForPlayer(const ULocalPlayer* LocalPlayer, FGameplayTag LayerName, TSubclassOf<UCommonActivatableWidget> WidgetClass)
{
	// LocalPlayer를 통해, GameUIManagerSubsystem을 가져옴
	UGameUIManagerSubsystem* UIManager = LocalPlayer->GetGameInstance()->GetSubsystem<UGameUIManagerSubsystem>();
	if (IsValid(UIManager))
	{
		// UIManager 에서 현재 활성화된 UIPolicy를 가져옴
		UGameUIPolicy* Policy = UIManager->GetCurrentUIPolicy();
		if (IsValid(Policy))
		{
			// Policy에서 LocalPlayer에 맞는 PrimaryGameLayout을 가져옴
			UPrimaryGameLayout* RootLayout = Policy->GetRootLayout(CastChecked<UCommonLocalPlayer>(LocalPlayer));
			if (IsValid(RootLayout))
			{
				// PrimaryGameLayout, WB_OverallUILayout의 LayerName에 Stack으로 WidgetClass(UCommonActivatableWidget)를 넣어줌
				return RootLayout->PushWidgetToLayerStack(LayerName, WidgetClass);
			}
		}
	}

	return nullptr;
}
