// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyLyraInventoryItemDefinition.generated.h"

/**
 * Inventory에 대한 Fragment 란?
 * - Lyra에서 사용하는 예시
 *		- ULyraInventoryFragment EquippableItem은 EquipementItemDefinition을 가지고 있으며, 장착 가능한 아이템을 의미
 *		- ULyraInventoryFragment SetStats는 아이템에 대한 정보를 가지고 있음
 *		- Rifle에 대한 SetStats으로 총알(Ammo)에 대한 장착 최대치와 현재 남은 잔탄 수를 예시로 들 수 있음
 *
 *	여기선 아이템에 부착이 가능한 Component와 비슷하게 여기면 그나마 쉽다.
 */
UCLASS(Abstract, DefaultToInstanced, EditInlineNew)
class UMyLyraInventoryItemFragment : public UObject
{
	GENERATED_BODY()
	
public:
	
};

/**
 * 
 */
UCLASS()
class MYLYRA_API UMyLyraInventoryItemDefinition : public UObject
{
	GENERATED_BODY()
public:
	UMyLyraInventoryItemDefinition(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	/** Inventory Item 정의(메타) 이름 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Display)
	FText DisplayName;

	/** Inventory Item의 Component를 Fragment로 인식하면 됨 */
	UPROPERTY(EditDefaultsOnly, Instanced, BlueprintReadOnly, Category = Display)
	TArray<TObjectPtr<UMyLyraInventoryItemFragment>> Fragments;
};
