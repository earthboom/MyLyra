// Fill out your copyright notice in the Description page of Project Settings.

#include "MyLyraQuickBarComponent.h"
#include "MyLyraEquipmentDefinition.h"
#include "MyLyraEquipmentInstance.h"
#include "MyLyraEquipmentManagerComponent.h"
#include "MyLyra/Inventory/MyLyraInventoryFragment_EquippableItem.h"
#include "MyLyra/Inventory/MyLyraInventoryItemInstance.h"

UMyLyraQuickBarComponent::UMyLyraQuickBarComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UMyLyraQuickBarComponent::BeginPlay()
{
	if (Slots.Num() < NumSlots)
	{
		Slots.AddDefaulted(NumSlots - Slots.Num());
	}

	Super::BeginPlay();
}

UMyLyraEquipmentManagerComponent* UMyLyraQuickBarComponent::FindEquipmentManager() const
{
	AController* OwnerController = Cast<AController>(GetOwner());
	if (IsValid(OwnerController))
	{
		APawn* Pawn = OwnerController->GetPawn();
		if (IsValid(Pawn))
		{
			return Pawn->FindComponentByClass<UMyLyraEquipmentManagerComponent>();
		}
	}
	return nullptr;
}

void UMyLyraQuickBarComponent::UnequipItemInSlot()
{
	// QuickBar는 Controller에 붙어있는 Component이지만, EquipmentManagerComponent는 Controller가 소유(Possess)하고 있는 Pawn의 Component
	UMyLyraEquipmentManagerComponent* EquipmentManager = FindEquipmentManager();
	if (IsValid(EquipmentManager))
	{
		// Pawn장비 해제
		EquipmentManager->UnequipmentItem(EquippedItem);
		// Controller에도 EquippedItem의 상태를 없는 것으로 업데이트
		EquippedItem = nullptr;
	}
}

void UMyLyraQuickBarComponent::EquipItemInSlot()
{
	check(Slots.IsValidIndex(ActiveSlotIndex));
	check(EquippedItem == nullptr);

	// 현재 활성화된 ActiveSlotIndex를 활용해, 활성화된 InventoryItemInstance를 찾음
	UMyLyraInventoryItemInstance* SlotItem = Slots[ActiveSlotIndex];
	if (IsValid(SlotItem))
	{
		// Slot Item을 통해 (InventoryItemInstance) InventoryFragment_EquippableItem의 Fragment를 찾음
		// - 찾는 것이 실패하면 장착할 수 없는 Inventory Item임을 의미
		const UMyLyraInventoryFragment_EquippableItem* EquipInfo = SlotItem->FindFragmentByClass<UMyLyraInventoryFragment_EquippableItem>();
		if (IsValid(EquipInfo))
		{
			// EquippableItem에서 EquipmentDefinition을 찾음
			// - EquipmentDefinition이 있어야, 장창 가능
			TSubclassOf<UMyLyraEquipmentDefinition> EquipDef = EquipInfo->EquipmentDefinition;
			if (IsValid(EquipDef))
			{
				// 아래는 Unequip랑 비슷하게 EquipmentManager 를 통해 장착
				UMyLyraEquipmentManagerComponent* EquipmentManager = FindEquipmentManager();
				if (IsValid(EquipmentManager))
				{
					EquippedItem = EquipmentManager->EquipItem(EquipDef);
					if (IsValid(EquippedItem))
					{
						// EquippedItem에는 앞서 봤던 Instigator로 Slot을 대상으로 넣음
						EquippedItem->Instigator = SlotItem;
					}
				}
			}
		}
	}
}

void UMyLyraQuickBarComponent::AddItemToSlot(int32 SlotIndex, UMyLyraInventoryItemInstance* Item)
{
	/**
	 * Slots는 Add로 동적 추가가 아닌, Index에 바로 넣음
	 * - 미리 Pre-Size 했다는 것이고 BeginPlay에서 진햏함
	 */
	if (Slots.IsValidIndex(SlotIndex) && (Item != nullptr))
	{
		if (Slots[SlotIndex] == nullptr)
		{
			Slots[SlotIndex] = Item;
		}
	}
}

void UMyLyraQuickBarComponent::SetActiveSlotIndex(int32 NewIndex)
{
	if (Slots.IsValidIndex(NewIndex) && (ActiveSlotIndex != NewIndex))
	{
		// UnequipItem / EquipItem 을 통해, NewIndex를 통해 할당된 Item을 장착 및 업데이트를 진행
		UnequipItemInSlot();
		ActiveSlotIndex = NewIndex;
		EquipItemInSlot();
	}
}
