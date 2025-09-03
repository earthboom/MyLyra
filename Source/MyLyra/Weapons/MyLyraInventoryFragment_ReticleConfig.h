// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyLyra/Inventory/MyLyraInventoryItemDefinition.h"
#include "MyLyraInventoryFragment_ReticleConfig.generated.h"

class UMyLyraReticleWidgetBase;

UCLASS()
class MYLYRA_API UMyLyraInventoryFragment_ReticleConfig : public UMyLyraInventoryItemFragment
{
	GENERATED_BODY()

public:
	/** 무기에 결합된 ReticleWidget 정보를 가지고 있는 Fragment */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Reticle")
	TArray<TSubclassOf<UMyLyraReticleWidgetBase>> ReticleWidgets; 
};