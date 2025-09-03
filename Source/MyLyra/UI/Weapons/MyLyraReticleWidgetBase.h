// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "MyLyraReticleWidgetBase.generated.h"

class UMyLyraInventoryItemInstance;
class UMyLyraWeaponInstance;

/**
 * 
 */
UCLASS(Abstract)
class MYLYRA_API UMyLyraReticleWidgetBase : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	UMyLyraReticleWidgetBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable)
	void InitializeFromWeapon(UMyLyraWeaponInstance* InWeapon);

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UMyLyraWeaponInstance> WeaponInstance;

	/** Fragment를 가져오기 위한 수단 */
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UMyLyraInventoryItemInstance> InventoryItemInstance;
};
