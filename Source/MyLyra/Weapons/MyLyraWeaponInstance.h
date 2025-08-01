// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyLyra/Cosmetics/MyLyraCosmeticAnimationTypes.h"
#include "MyLyra/Equipment/MyLyraEquipmentInstance.h"
#include "MyLyraWeaponInstance.generated.h"

/**
 * 
 */
UCLASS()
class MYLYRA_API UMyLyraWeaponInstance : public UMyLyraEquipmentInstance
{
	GENERATED_BODY()

public:
	UMyLyraWeaponInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** Weapon에 Equip / Unequip에 대한 Animation Set 정보를 들고 있음 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation)
	FMyLyraAnimLayerSelectionSet EquippedAnimSet;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation)
	FMyLyraAnimLayerSelectionSet UnequippedAnimSet;
};
