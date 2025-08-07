// Fill out your copyright notice in the Description page of Project Settings.

#include "MyLyraEquipmentManagerComponent.h"
#include "AbilitySystemGlobals.h"
#include "MyLyraEquipmentDefinition.h"
#include "MyLyraEquipmentInstance.h"
#include "MyLyra/AbilitySystem/MyLyraAbilitySystemComponent.h"

UMyLyraEquipmentInstance* FMyLyraEquipmentList::AddEntry(TSubclassOf<UMyLyraEquipmentDefinition> EquipmentDef)
{
	UMyLyraEquipmentInstance* Result = nullptr;
	check(IsValid(EquipmentDef));
	check(IsValid(OwnerComponent));
	check(OwnerComponent->GetOwner()->HasAuthority());

	// EquipmentDefinition의 멤버 변수들은 EditDefaultsOnly로 정의되어 있어 GetDefault로 돌고 와도 우리에게 필요한 것들이 모두 들어있음
	const UMyLyraEquipmentDefinition* EquipmentCDO = GetDefault<UMyLyraEquipmentDefinition>(EquipmentDef);

	TSubclassOf<UMyLyraEquipmentInstance> InstanceType = EquipmentCDO->InstanceType;
	if (IsValid(InstanceType) == false)
	{
		InstanceType = UMyLyraEquipmentInstance::StaticClass();
	}

	// Entries에 추가
	FMyLyraAppliedEquipmentEntry& NewEntry = Entries.AddDefaulted_GetRef();
	NewEntry.EquipmentDefinition = EquipmentDef;
	NewEntry.Instance = NewObject<UMyLyraEquipmentInstance>(OwnerComponent->GetOwner(), InstanceType);
	Result = NewEntry.Instance;

	UMyLyraAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	check(ASC);
	{
		for (TObjectPtr<UMyLyraAbilitySet> AbilitySet : EquipmentCDO->AbilitySetsGrant)
		{
			AbilitySet->GiveToAbilitySystem(ASC, &NewEntry.GrantedHandles, Result);
		}
	}

	// ActorsToSpawn을 통해, Actor들을 인스턴스화
	// - EquipmentInstance 에
	Result->SpawnEquipmentActors(EquipmentCDO->ActorToSpawn);

	return Result;
}

void FMyLyraEquipmentList::RemoveEntry(UMyLyraEquipmentInstance* Instance)
{
	TIndexedContainerIterator<TArray<FMyLyraAppliedEquipmentEntry>, FMyLyraAppliedEquipmentEntry, int32> EntryIt = Entries.CreateIterator();
	for (; EntryIt; ++EntryIt)
	{
		FMyLyraAppliedEquipmentEntry& Entry = *EntryIt;
		if (Entry.Instance == Instance)
		{
			UMyLyraAbilitySystemComponent* ASC = GetAbilitySystemComponent();
			check(ASC);
			{
				// TakeFromAbilitySystem은 GiveToAbilitySystem의 반대 역할, ActivatableAbilities 를 제거
				Entry.GrantedHandles.TakeFromAbilitySystem(ASC);
			}

			// Actor 제거 작업 및 Iterator를 통한 안전하게 Array에서 제거 진행
			Instance->DestroyEquipmentActors();
			EntryIt.RemoveCurrent();
		}
	}
}

UMyLyraAbilitySystemComponent* FMyLyraEquipmentList::GetAbilitySystemComponent() const
{
	check(OwnerComponent);
	AActor* OwningActor = OwnerComponent->GetOwner();

	/**
	 * GetAbilitySYstemComponentFromActor
	 * - EquipmentManagerComponent는 AMyLyraCharacter를 Owner로 가짐
	 * - 해당 함수는 IAbilitySYstemInterface를 통해 AbilitySYstemCOmponent를 반환
	 * - MyLyraCharacter에 IAbilitySystemInterface를 상속받을 필요가 있음
	 */
	return Cast<UMyLyraAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OwningActor));
}

UMyLyraEquipmentManagerComponent::UMyLyraEquipmentManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	  , EquipmentList(this)
{
}

UMyLyraEquipmentInstance* UMyLyraEquipmentManagerComponent::EquipItem(TSubclassOf<UMyLyraEquipmentDefinition> EquipmentDefinition)
{
	UMyLyraEquipmentInstance* Result = nullptr;
	if (IsValid(EquipmentDefinition))
	{
		Result = EquipmentList.AddEntry(EquipmentDefinition);
		if (IsValid(Result))
		{
			Result->OnEquipped();
		}
	}
	return Result;
}

void UMyLyraEquipmentManagerComponent::UnequipmentItem(UMyLyraEquipmentInstance* ItemInstance)
{
	if (IsValid(ItemInstance))
	{
		// 해당 함수는 BP의 Event 노드를 호출해 줌
		ItemInstance->OnUnequipped();

		// EquipmentList에 제거해줌
		// - 제거하는 과정을 통해 추가되었던 Actor Instance를 제거 진행
		EquipmentList.RemoveEntry(ItemInstance);
	}
}

TArray<UMyLyraEquipmentInstance*> UMyLyraEquipmentManagerComponent::GetEquipmentInstanceofType(TSubclassOf<UMyLyraEquipmentInstance> InstanceType) const
{
	TArray<UMyLyraEquipmentInstance*> Results;

	// EquipmentList 순회
	for (const FMyLyraAppliedEquipmentEntry& Entry : EquipmentList.Entries)
	{
		UMyLyraEquipmentInstance* Instance = Entry.Instance;
		if (IsValid(Instance))
		{
			// InstanceType에 맞는 Class이면 Results에 추가
			// - MyLyraRangedWeaponInstance
			if (Instance->IsA(InstanceType))
			{
				Results.Add(Instance);
			}
		}
	}

	return Results;
}
