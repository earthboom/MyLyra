// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyLyraActivatableWidget.h"
#include "MyLyraHUDLayout.generated.h"

/**
 * PrimaryGameLayout의 Layer에 연동할 HUD Layout (CommonActivatableWidget)
 */
UCLASS(Abstract, BlueprintType, Blueprintable, meta = (DisplayName = "MyLyra HUD Layout", Category = "MyLyra|HUD"))
class MYLYRA_API UMyLyraHUDLayout : public UMyLyraActivatableWidget
{
	GENERATED_BODY()

public:
	UMyLyraHUDLayout(const FObjectInitializer& ObjectInitializer);
};
