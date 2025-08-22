// Fill out your copyright notice in the Description page of Project Settings.

#include "MyLyraActivatableWidget.h"
#include "CommonInputModeTypes.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(MyLyraActivatableWidget)

UMyLyraActivatableWidget::UMyLyraActivatableWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

TOptional<FUIInputConfig> UMyLyraActivatableWidget::GetDesiredInputConfig() const
{
	/**
	 * 정의한 WidgetInputMode에 따라 InputConfig 설정
	 *	- ECommonInputMode에 따라 Input의 흐름을 Game/Menu 혹은 둘 다로 정하는 것이 가능
	 */
	switch (InputConfig)
	{
	case EMyLyraWidgetInputMode::GameAndMenu:
		return FUIInputConfig(ECommonInputMode::All, GameMouseCaptureMode);
	case EMyLyraWidgetInputMode::Game:
		return FUIInputConfig(ECommonInputMode::Game, GameMouseCaptureMode);
	case EMyLyraWidgetInputMode::Menu:
		return FUIInputConfig(ECommonInputMode::Menu, EMouseCaptureMode::NoCapture);
	case EMyLyraWidgetInputMode::Default:
	default:
		return TOptional<FUIInputConfig>();
	}
}
