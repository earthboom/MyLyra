// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MyLyraInventoryManagerComponent.generated.h"

class UMyLyraInventoryItemInstance;
class UMyLyraInventoryItemDefinition;

/** Inventory Item 단위 객체 */
USTRUCT( BlueprintType)
struct FMyLyraInventoryEntry
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<UMyLyraInventoryItemInstance> Instance = nullptr;
};

/** Inventory Item 관리 객체 */
USTRUCT(BlueprintType)
struct FMyLyraInventoryList
{
	GENERATED_BODY()

	FMyLyraInventoryList(UActorComponent* InOwnerComponent = nullptr)
		: OwnerComponent(InOwnerComponent)
	{}

	UPROPERTY()
	TArray<FMyLyraInventoryEntry> Entries;

	UPROPERTY()
	TObjectPtr<UActorComponent> OwnerComponent;
};

/**
 * PlayerController의 Component로서 Inventory를 관리
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MYLYRA_API UMyLyraInventoryManagerComponent : public UActorComponent
{
	GENERATED_BODY()
	
public:
	UMyLyraInventoryManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	UPROPERTY()
	FMyLyraInventoryList InventoryList;
};
