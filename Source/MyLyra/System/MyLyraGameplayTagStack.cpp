// Fill out your copyright notice in the Description page of Project Settings.

#include "MyLyraGameplayTagStack.h"

void FMyLyraGameplayTagStackContainer::AddStack(FGameplayTag Tag, int32 StackCount)
{
	if (Tag.IsValid() == false)
	{
		return;
	}

	if (StackCount > 0)
	{
		for (FMyLyraGameplayTagStack& Stack : Stacks)
		{
			if (Stack.Tag == Tag)
			{
				const int32 NewCount = Stack.StackCount + StackCount;
				Stack.StackCount = NewCount;
				TagToCountMap[Tag] = NewCount;
				return;
			}
		}

		FMyLyraGameplayTagStack& NewStack = Stacks.Emplace_GetRef(Tag, StackCount);

		TagToCountMap.Add(Tag, StackCount);
	}
}

void FMyLyraGameplayTagStackContainer::RemoveStack(FGameplayTag Tag, int32 StackCount)
{
	if (Tag.IsValid() == false)
	{
		return;
	}

	if (StackCount > 0)
	{
		for (auto It = Stacks.CreateIterator(); It; ++It)
		{
			FMyLyraGameplayTagStack& Stack = *It;
			if (Stack.Tag == Tag)
			{
				if (Stack.StackCount <= StackCount)
				{
					It.RemoveCurrent();
					TagToCountMap.Remove(Tag);
				}
				else
				{
					const int32 NewCount = Stack.StackCount - StackCount;
					Stack.StackCount = NewCount;
					TagToCountMap[Tag] = NewCount;
				}
				return;
			}
		}
	}
}
