// Fill out your copyright notice in the Description page of Project Settings.

#include "MyLyraReticleWidgetBase.h"
#include "MyLyra/Inventory/MyLyraInventoryItemInstance.h"
#include "MyLYra/Weapons/MyLyraWeaponInstance.h"

UMyLyraReticleWidgetBase::UMyLyraReticleWidgetBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UMyLyraReticleWidgetBase::InitializeFromWeapon(UMyLyraWeaponInstance* InWeapon)
{
	WeaponInstance = InWeapon;
	InventoryItemInstance = nullptr;
	if (WeaponInstance)
	{
		InventoryItemInstance = Cast<UMyLyraInventoryItemInstance>(WeaponInstance->GetInstigator());
	}

	OnWeaponInitialized();
}
