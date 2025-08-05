// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PawnComponent.h"
#include "MyLyra/AbilitySystem/MyLyraAbilitySet.h"
#include "MyLyraEquipmentManagerComponent.generated.h"

class UMyLyraEquipmentInstance;
class UMyLyraEquipmentDefinition;

USTRUCT(BlueprintType)
struct FMyLyraAppliedEquipmentEntry
{
	GENERATED_BODY()

	/** 장착물에 대한 메타 데이터 */
	UPROPERTY()
	TSubclassOf<UMyLyraEquipmentDefinition> EquipmentDefinition;

	/** EquipmentDefinition을 통해 생성되는 인스턴스 */
	UPROPERTY()
	TObjectPtr<UMyLyraEquipmentInstance> Instance = nullptr;

	/** 무기에 할당된 허용 가능한 GameplayAbility */
	UPROPERTY()
	FMyLyraAbilitySet_GrantedHandles GrantedHandles;
};

/**
 * EquipmentInstance의 인스턴스를 Entry에서 관리하고 있음
 * - MyLyraEquipmentList는 생성된 객체를 관리한다고 보면 됨
 */
USTRUCT(BlueprintType)
struct FMyLyraEquipmentList
{
	GENERATED_BODY()

	FMyLyraEquipmentList(UActorComponent* InOwnerComponent = nullptr)
		: OwnerComponent(InOwnerComponent)
	{
	}

	UMyLyraEquipmentInstance* AddEntry(TSubclassOf<UMyLyraEquipmentDefinition> EquipmentDef);
	void RemoveEntry(UMyLyraEquipmentInstance* Instance);

	UMyLyraAbilitySystemComponent* GetAbilitySystemComponent() const;

	/** 창작물에 대한 관리 리스트 */
	UPROPERTY()
	TArray<FMyLyraAppliedEquipmentEntry> Entries;

	UPROPERTY()
	TObjectPtr<UActorComponent> OwnerComponent;
};

/**
 * Pawn의 Component로서 장착물에 대한 관리를 담당
 */
UCLASS()
class MYLYRA_API UMyLyraEquipmentManagerComponent : public UPawnComponent
{
	GENERATED_BODY()

public:
	UMyLyraEquipmentManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UMyLyraEquipmentInstance* EquipItem(TSubclassOf<UMyLyraEquipmentDefinition> EquipmentDefinition);
	void UnequipmentItem(UMyLyraEquipmentInstance* ItemInstance);
	
	UPROPERTY()
	FMyLyraEquipmentList EquipmentList;
};
