// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ControllerComponent.h"
#include "MyLyraQuickBarComponent.generated.h"

class UMyLyraEquipmentManagerComponent;
class UMyLyraEquipmentInstance;
class UMyLyraInventoryItemInstance;
/**
 * HUD의 QuickBar
 * - 흔히 MMORPG에선 ShortCut HUD
 *
 * 해당 Component는 ControllerComponent로서, PlayerController에 의해 조작계 중 하나로 여겨라
 * - HUD (Slate) 와 Inventory / Equipment (Gameplay)의 다리(Bridge) 역할하는 Component로 생각
 * - 해당 Component는 Lyra의 HUD 및 Slate Widget을 다루면서 다시 볼 예정
 */
UCLASS()
class MYLYRA_API UMyLyraQuickBarComponent : public UControllerComponent
{
	GENERATED_BODY()

public:
	UMyLyraQuickBarComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;

	UMyLyraEquipmentManagerComponent* FindEquipmentManager() const;
	void UnequipItemInSlot();
	void EquipItemInSlot();
	
	UFUNCTION(BlueprintCallable)
	void AddItemToSlot(int32 SlotIndex, UMyLyraInventoryItemInstance* Item);

	UFUNCTION(BlueprintCallable, Category = "MyLyra")
	void SetActiveSlotIndex(int32 NewIndex);

	/** HUD QuickBar Slot 개수 */
	UPROPERTY()
	int32 NumSlots = 3;

	/** HUD QuickBar Slot 리스트 */
	UPROPERTY()
	TArray<TObjectPtr<UMyLyraInventoryItemInstance>> Slots;

	/** 현재 활성화된 Slot Index */
	UPROPERTY()
	int32 ActiveSlotIndex = -1;

	/** 현재 장착한 장비 정보 */
	UPROPERTY()
	TObjectPtr<UMyLyraEquipmentInstance> EquippedItem;
};
