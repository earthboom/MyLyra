// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "MyLyraInventoryItemDefinition.h"
#include "MyLyraInventoryFragment_SetStats.generated.h"

/**
 * 
 */
UCLASS()
class MYLYRA_API UMyLyraInventoryFragment_SetStats : public UMyLyraInventoryItemFragment
{
	GENERATED_BODY()

public:
	virtual void OnInstanceCreated(UMyLyraInventoryItemInstance* Instance) const override;

	/** InitialItemStats은 MyLyraGameplayTagStackContainer에 대한 생성자의 매개변수를 제공 */
	UPROPERTY(EditDefaultsOnly, Category = Equipment)
	TMap<FGameplayTag, int32> InitializeItemStats;
};
