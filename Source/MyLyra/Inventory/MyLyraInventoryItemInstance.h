// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "MyLyraInventoryItemInstance.generated.h"

class UMyLyraInventoryItemFragment;
class UMyLyraInventoryItemDefinition;

/**
 * 
 */
UCLASS()
class MYLYRA_API UMyLyraInventoryItemInstance : public UObject
{
	GENERATED_BODY()

public:
	UMyLyraInventoryItemInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	const UMyLyraInventoryItemFragment* FindFragmentByClass(TSubclassOf<UMyLyraInventoryItemFragment> FragmentClass) const;

	template <typename ResultClass>
	const ResultClass* FindFragmentByClass() const
	{
		return (ResultClass*)FindFragmentByClass(ResultClass::StaticClass());
	}

	/** Inventory Item의 인스턴스에는 무엇으로 정의되었는지 메타 클래스인 MyLyraInventoryItemDefinition을 들고 있음 */
	UPROPERTY()
	TSubclassOf<UMyLyraInventoryItemDefinition> ItemDef;
};
