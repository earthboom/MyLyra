// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "MyLyraActivatableWidget.generated.h"

struct FUIInputConfig;

/**
 * Input 처리 방식 정의
 */
UENUM(BlueprintType)
enum class EMyLyraWidgetInputMode : uint8
{
	Default,
	GameAndMenu,
	Game,
	Menu,
};

/**
 * CommonActivatableWidget 특성
 * 1. Widget Layout과 Widget Instance를 구분하기 위한 용도로 CommonActivatableWidget은 Layout 정의
 *	- 런타임 Activate / Deactivate
 *	- WidgetTree에서 제거가 아닌 On / Off ( == Activate/Deactivate)
 * 2. Input (Mouse or Keyboard etc .... ) 처리 방법 정의 
 */
UCLASS()
class MYLYRA_API UMyLyraActivatableWidget : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	UMyLyraActivatableWidget(const FObjectInitializer& ObjectInitializer);

	virtual TOptional<FUIInputConfig> GetDesiredInputConfig() const override;

	/** Input 처리 방식 */
	UPROPERTY(EditDefaultsOnly, Category = Input)
	EMyLyraWidgetInputMode InputConfig = EMyLyraWidgetInputMode::Default;

	/** Mouse 처리 방식 */
	UPROPERTY(EditDefaultsOnly, Category = Input)
	EMouseCaptureMode GameMouseCaptureMode = EMouseCaptureMode::CapturePermanently;
};
