// Fill out your copyright notice in the Description page of Project Settings.

#include "MyLyraAbilityCost_ItemTagStack.h"
#include "NativeGameplayTags.h"
#include "MyLyra/Equipment/MyLyraGameplayAbility_FromEquipment.h"
#include "MyLyra/Inventory/MyLyraInventoryItemInstance.h"

UE_DEFINE_GAMEPLAY_TAG(TAG_ABILITY_FAIL_COST, "Ability.ActivateFail.Cost")

UMyLyraAbilityCost_ItemTagStack::UMyLyraAbilityCost_ItemTagStack()
	: Super()
{
	Quantity.SetValue(1.0f);
	FailureTag = TAG_ABILITY_FAIL_COST;
}

bool UMyLyraAbilityCost_ItemTagStack::CheckCost(const UMyLyraGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) const
{
	// Item을 장비한 경우에만 cost를 확인함
	if (const UMyLyraGameplayAbility_FromEquipment* EquipmentAbility = Cast<const UMyLyraGameplayAbility_FromEquipment>(Ability))
	{
		if (UMyLyraInventoryItemInstance* ItemInstance = EquipmentAbility->GetAssociatedItem())
		{
			const int32 AbilityLevel = Ability->GetAbilityLevel(Handle, ActorInfo);

			/**
			 * 현재, 무기는 권총 뿐
			 * 깊은 이해를 위해 무기를 마법 권총으로 생각해 보자
			 * - 마법 권총의 가격은 총알 두발. 무기 Lv2일 때 강력한 한 방을 제공
			 * - 마법 권총 Lv5는 총알 4발을 비용으로 더 강력한 한 방을 제공
			 */
			const float NumStackReal = Quantity.GetValueAtLevel(AbilityLevel);
			const int32 NumStacks = FMath::TruncToInt(NumStackReal);
			const bool bCanApplyCost = ItemInstance->GetStatTagStackCount(Tag) >= NumStacks;

			// 실패 사유를 남긴다
			if (bCanApplyCost == false && OptionalRelevantTags && FailureTag.IsValid())
			{
				OptionalRelevantTags->AddTag(FailureTag);
			}
			return bCanApplyCost;
		}
	}

	return false;
}

void UMyLyraAbilityCost_ItemTagStack::ApplyCost(const UMyLyraGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
{
	if (const UMyLyraGameplayAbility_FromEquipment* EquipmentAbility = Cast<const UMyLyraGameplayAbility_FromEquipment>(Ability))
	{
		if (UMyLyraInventoryItemInstance* ItemInstance = EquipmentAbility->GetAssociatedItem())
		{
			const int32 AbilityLevel = Ability->GetAbilityLevel(Handle, ActorInfo);
			const float NumStackReal = Quantity.GetValueAtLevel(AbilityLevel);
			const int32 NumStacks = FMath::TruncToInt(NumStackReal);

			// InventoryItemInstance 에서 stat tag 수 감소
			ItemInstance->RemoveStatTagStack(Tag, NumStacks);
		}
	}
}
