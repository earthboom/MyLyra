// Fill out your copyright notice in the Description page of Project Settings.

#include "MyLyraGameplayAbility_FromEquipment.h"
#include "MyLyraEquipmentInstance.h"
#include "MyLyra/Inventory/MyLyraInventoryItemInstance.h"

UMyLyraEquipmentInstance* UMyLyraGameplayAbility_FromEquipment::GetAssociatedEquipment() const
{
	/**
	 * CurrentActorInfo의 AbilitySystemComponent와 CurrentSpecHandle을 활용해, GameplayAbilitySpec을 가져옴;
	 * - CurrentSpecHandle은 SetCurrentActorInfo() 호출할 때, Handle 값을 받아 저장됨
	 * - CurrentSpecHandle과 CurrentActorInfo는 같이함
	 * - FindAbilitySpecFromHandle은 GiveAbility로 허용된 ActivatableAbilities를 순회하며 GameplayAbilitySpec을 찾아냄
	 */
	FGameplayAbilitySpec* Spec = UGameplayAbility::GetCurrentAbilitySpec();
	if (Spec)
	{
		// GameplayAbility_FromEquipment는 EquipmentInstance로부터 GiveAbility를 진행했으므로, SourceObject에 EquipmentInstance가 저장되어 있음
		return Cast<UMyLyraEquipmentInstance>(Spec->SourceObject.Get());
	}
	return nullptr;
}

UMyLyraInventoryItemInstance* UMyLyraGameplayAbility_FromEquipment::GetAssociatedItem() const
{
	if (UMyLyraEquipmentInstance* Equipment = GetAssociatedEquipment())
	{
		/**
		 * Lyra 에선 장비가 InventoryItemInstance에 의해 장착되어 있음
		 * - 그래서 장비의 Instigator는 InventoryItemInstance 여야 함
		 * - 그렇지 않으면 UMyLyraInventoryItemInstance에 캐스팅 실패로 nullptr 반환
		 */
		return Cast<UMyLyraInventoryItemInstance>(Equipment->GetInstigator());
	}
	
	return nullptr;
}
