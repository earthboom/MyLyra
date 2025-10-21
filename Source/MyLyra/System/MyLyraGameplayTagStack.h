// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameplayTagContainer.h"
#include "MyLyraGameplayTagStack.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FMyLyraGameplayTagStack
{
	GENERATED_BODY()

	FMyLyraGameplayTagStack()
	{
	}

	FMyLyraGameplayTagStack(FGameplayTag InTag, int32 InStackCount)
		: Tag(InTag)
		  , StackCount(InStackCount)
	{
	}

	UPROPERTY()
	FGameplayTag Tag;

	UPROPERTY()
	int32 StackCount = 0;
};

USTRUCT(BlueprintType)
struct FMyLyraGameplayTagStackContainer
{
	GENERATED_BODY()

	FMyLyraGameplayTagStackContainer()
	{
	}

	void AddStack(FGameplayTag Tag, int32 StackCount);
	void RemoveStack(FGameplayTag Tag, int32 StackCount);

	int32 GetStackCount(FGameplayTag Tag) const
	{
		return TagToCountMap.FindRef(Tag);
	}

	bool ContainsTag(FGameplayTag Tag) const
	{
		return TagToCountMap.Contains(Tag);
	}

	UPROPERTY()
	TArray<FMyLyraGameplayTagStack> Stacks;

	TMap<FGameplayTag, int32> TagToCountMap;
};
