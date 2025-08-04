// Fill out your copyright notice in the Description page of Project Settings.

#include "MyLyraAbilitySet.h"
#include "MyLyraAbilitySystemComponent.h"
#include "Abilities/MyLyraGameplayAbility.h"

void FMyLyraAbilitySet_GrantedHandles::AddAbilitySpecHande(const FGameplayAbilitySpecHandle& Handle)
{
	if (Handle.IsValid())
	{
		AbilitySpecHandles.Add(Handle);
	}
}

UMyLyraAbilitySet::UMyLyraAbilitySet(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UMyLyraAbilitySet::GiveToAbilitySystem(UMyLyraAbilitySystemComponent* MyLyraASC, FMyLyraAbilitySet_GrantedHandles* OutGrantedHandles, UObject* SourceObject)
{
	check(IsValid(MyLyraASC));

	if (MyLyraASC->IsOwnerActorAuthoritative() == false)
	{
		return;
	}

	// Gameplay Abilities 를 허용
	for (int32 AbilityIndex = 0; AbilityIndex < GrantedGameplayAbilities.Num(); ++AbilityIndex)
	{
		const FMyLyraAbilitySet_GameplayAbility& AbilityToGrant = GrantedGameplayAbilities[AbilityIndex];
		if (IsValid(AbilityToGrant.Ability) == false)
		{
			continue;
		}

		// GiveAbility()에서 만약 EGameplayAbilityInstancingPolicy::InstancedPerActor라면, 내부적으로 Instance를 생성하지만 그렇지 않으면 CDO를 할당
		// - 이를 통해 UObject 개수를 줄여 UObject가 늘어남에 따라 늘어나는 성능 / 메모리 부하를 줄일 수 있음
		UMyLyraGameplayAbility* AbilityCDO = AbilityToGrant.Ability->GetDefaultObject<UMyLyraGameplayAbility>();

		// AbilitySpec은 GiveAbility로 전달되어, ActivatableAbilities에 추가
		FGameplayAbilitySpec AbilitySpec(AbilityCDO, AbilityToGrant.AbilityLevel);
		AbilitySpec.SourceObject = SourceObject;
		AbilitySpec.DynamicAbilityTags.AddTag(AbilityToGrant.InputTag);

		// GiveAbility()
		const FGameplayAbilitySpecHandle AbilitySpecHandle = MyLyraASC->GiveAbility(AbilitySpec);
		if (OutGrantedHandles)
		{
			OutGrantedHandles->AddAbilitySpecHande(AbilitySpecHandle);
		}
	}
}
