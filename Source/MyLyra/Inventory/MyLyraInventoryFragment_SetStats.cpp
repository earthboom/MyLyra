// Fill out your copyright notice in the Description page of Project Settings.

#include "MyLyraInventoryFragment_SetStats.h"
#include "MyLyraInventoryItemInstance.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(MyLyraInventoryFragment_SetStats)

void UMyLyraInventoryFragment_SetStats::OnInstanceCreated(UMyLyraInventoryItemInstance* Instance) const
{
	// InitialItemStats의 Iterating 및 InventoryItemInstance에 통계 태그 추가
	for (const TPair<FGameplayTag, int32>& InitialItemStat : InitializeItemStats)
	{
		Instance->AddStatTagStack(InitialItemStat.Key, InitialItemStat.Value);
	}
}
