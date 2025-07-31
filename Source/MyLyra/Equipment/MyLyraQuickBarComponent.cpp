// Fill out your copyright notice in the Description page of Project Settings.

#include "MyLyraQuickBarComponent.h"

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
