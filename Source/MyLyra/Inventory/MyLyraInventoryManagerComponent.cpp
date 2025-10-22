// Fill out your copyright notice in the Description page of Project Settings.

#include "MyLyraInventoryManagerComponent.h"
#include "MyLyraInventoryItemDefinition.h"
#include "MyLyraInventoryItemInstance.h"

UMyLyraInventoryItemInstance* FMyLyraInventoryList::AddEntry(TSubclassOf<UMyLyraInventoryItemDefinition> ItemDef)
{
	UMyLyraInventoryItemInstance* Result = nullptr;
	check(ItemDef);
	check(OwnerComponent);

	AActor* OwningActor = OwnerComponent->GetOwner();
	check(OwningActor->HasAuthority());

	FMyLyraInventoryEntry& NewEntry = Entries.AddDefaulted_GetRef();
	NewEntry.Instance = NewObject<UMyLyraInventoryItemInstance>(OwningActor);
	NewEntry.Instance->ItemDef = ItemDef;

	// Fragments를 iterating하고 OnInstanceCreate()로 Callback
	for (UMyLyraInventoryItemFragment* Fragment : GetDefault<UMyLyraInventoryItemDefinition>(ItemDef)->Fragments)
	{
		if (Fragment)
		{
			Fragment->OnInstanceCreated(NewEntry.Instance);
		}
	}

	Result = NewEntry.Instance;
	return Result;
}

UMyLyraInventoryManagerComponent::UMyLyraInventoryManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, InventoryList(this)
{
}

UMyLyraInventoryItemInstance* UMyLyraInventoryManagerComponent::AddItemDefinition(TSubclassOf<UMyLyraInventoryItemDefinition> ItemDef)
{
	UMyLyraInventoryItemInstance* Result = nullptr;
	if (IsValid(ItemDef))
	{
		Result = InventoryList.AddEntry(ItemDef);
	}
	return Result;
}
