// Fill out your copyright notice in the Description page of Project Settings.

#include "PrimaryGameLayout.h"
#include "Widgets/CommonActivatableWidgetContainer.h"

UPrimaryGameLayout::UPrimaryGameLayout(const FObjectInitializer& ObjectInitializer)
{
}

UCommonActivatableWidgetContainerBase* UPrimaryGameLayout::GetLayerWidget(FGameplayTag LayerName)
{
	return Layers.FindRef(LayerName);
}

void UPrimaryGameLayout::RegisterLayer(FGameplayTag LayerTag, UCommonActivatableWidgetContainerBase* LayerWidget)
{
	if (IsDesignTime() == false)
	{
		LayerWidget->SetTransitionDuration(0.0f);	// Widget 전환 속도 세팅
		Layers.Add(LayerTag, LayerWidget);
	}
}
