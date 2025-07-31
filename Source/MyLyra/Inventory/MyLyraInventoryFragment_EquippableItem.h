// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyLyraInventoryItemDefinition.h"
#include "MyLyraInventoryFragment_EquippableItem.generated.h"

class UMyLyraEquipmentDefinition;

/**
 * 
 */
UCLASS()
class MYLYRA_API UMyLyraInventoryFragment_EquippableItem : public UMyLyraInventoryItemFragment
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = MyLyra)
	TSubclassOf<UMyLyraEquipmentDefinition> EquipmentDefinition;
};
