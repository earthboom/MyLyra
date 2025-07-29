// Fill out your copyright notice in the Description page of Project Settings.


#include "MyLyraInventoryManagerComponent.h"

UMyLyraInventoryManagerComponent::UMyLyraInventoryManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, InventoryList(this)
{
}
