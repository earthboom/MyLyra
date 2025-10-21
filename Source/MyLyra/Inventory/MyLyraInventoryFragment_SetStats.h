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

	virtual void OnInstanceCreated(UMyLyraInventoryItemInstance* Instance) const override;
	
	UPROPERTY(EditDefaultsOnly, Category = Equipment)
	TMap<FGameplayTag, int32> InitializeItemStats;
};
