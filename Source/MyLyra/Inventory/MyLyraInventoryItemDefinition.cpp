// Fill out your copyright notice in the Description page of Project Settings.


#include "MyLyraInventoryItemDefinition.h"

UMyLyraInventoryItemDefinition::UMyLyraInventoryItemDefinition(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

const UMyLyraInventoryItemFragment* UMyLyraInventoryItemDefinition::FindFragmentByClass(TSubclassOf<UMyLyraInventoryItemFragment> FragmentClass) const
{
	if (IsValid(FragmentClass))
	{
		for (const UMyLyraInventoryItemFragment* Fragment : Fragments)
		{
			if (IsValid(Fragment) && Fragment->IsA(FragmentClass))
			{
				return Fragment;
			}
		}
	}
	return nullptr;
}
