// Fill out your copyright notice in the Description page of Project Settings.

#include "MyLyraInventoryItemInstance.h"
#include "MyLyraInventoryItemDefinition.h"

UMyLyraInventoryItemInstance::UMyLyraInventoryItemInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

const UMyLyraInventoryItemFragment* UMyLyraInventoryItemInstance::FindFragmentByClass(TSubclassOf<UMyLyraInventoryItemFragment> FragmentClass) const
{
	if ((IsValid(ItemDef)) && (IsValid(FragmentClass)))
	{
		// MyLyraInventoryItemDefinition은 모든 멤버 변수가 EditDefaultsOnly로 선언되어 있으므로, GetDefault로 가져와도 무관
		// - Fragment 정보는 Instance가 아닌 Definition에 있음
		return GetDefault<UMyLyraInventoryItemDefinition>(ItemDef)->FindFragmentByClass(FragmentClass);
	}
	return nullptr;
}
