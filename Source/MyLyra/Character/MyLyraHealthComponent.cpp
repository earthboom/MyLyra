// Fill out your copyright notice in the Description page of Project Settings.

#include "MyLyraHealthComponent.h"
#include "GameplayEffectExtension.h"
#include "MyLyra/MyLyraLogChannels.h"
#include "MyLyra/AbilitySystem/MyLyraAbilitySystemComponent.h"
#include "MyLyra/AbilitySystem/Attributes/MyLyraHealthSet.h"

UMyLyraHealthComponent::UMyLyraHealthComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// HealthComponent는 PlayerState의 HealthSet과 Character(Pawn) 사이의 Bridge역할
	// - 따로 로직이 업데이트 될 필요가 없는 이벤트 기반으로 동작하는 컴포넌트로 이해할 것
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;

	// InitializeWithAbilitySystem으로 ASC가 초기화되기 전까지 HealthSet과 ASC는 null
	AbilitySystemComponent = nullptr;
	HealthSet = nullptr;
}

void UMyLyraHealthComponent::InitializeWithAbilitySystem(UMyLyraAbilitySystemComponent* InASC)
{
	// AActor는 MyLyraCharacter를 상속받는 클래스일 것임
	AActor* Owner = GetOwner();
	check(Owner);

	if (AbilitySystemComponent)
	{
		UE_LOG(LogMyLyra, Error, TEXT("MyLyraHealthComponent: Health component for owner [%s] has already been initialized with an ability system."), *GetNameSafe(Owner));
		return;
	}

	AbilitySystemComponent = InASC;
	if (AbilitySystemComponent == nullptr)
	{
		UE_LOG(LogMyLyra, Error, TEXT("MyLyraHealthComponent: Cannot initialize health component for owner [%s] with NULL ability system."), *GetNameSafe(Owner));
		return;
	}

	// AbilitySystemComponent::GetSet은 SpawnAttributes에서 가져옮
	// - 근데 PlayereState에서 Subobject를 생성하고 따로 ASC에 등록된이 없는데 어떻ㅔ 드럭
	//	- AbilitysystemComponent::INsnitializeComponent()d에서 GeObjectLiWWithOuter로 SpawnedAttribute에 추가
	//  - 잘 생각해보자 HealthSet은 PlayerState에 SubObject로 있고, ASC또한 PlayerState가 있음
	//     -> 이는 ASC에에 GetObjectWithOuter로 HealthSet를 예시로 들 수 있음
	HealthSet = AbilitySystemComponent->GetSet<UMyLyraHealthSet>();
	if (HealthSet == nullptr)
	{
		UE_LOG(LogMyLyra, Error, TEXT("MyLyraHealthComponent: Cannot initialize health component for owner [%s] with NULL health set on the ability system."), * GetNameSafe(Owner));
		return;
	}

	// HealthSet의 HealthAttribute의 업데이트가 일어날 때마다 호출할 콜백으로 멤버에서도 HandLeHealthChaged를 등록
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UMyLyraHealthSet::GetHealthAttribute()).AddUObject(this, &ThisClass::HandleHealthChanged);

	// 초기화 한번 했으니, BroadCast 해줌
	OnHealthChanged.Broadcast(this, 0, HealthSet->GetHealth(), nullptr);
}

void UMyLyraHealthComponent::UninitializeWithAbilitySystem()
{
	AbilitySystemComponent = nullptr;
	HealthSet = nullptr;
}

static AActor* GetInstigatorFromAttrChangeData(const FOnAttributeChangeData& ChangeData)
{
	// GameEffectModifier에 Data가 있을 경우만 호출되는 기능
	if (ChangeData.GEModData != nullptr)
	{
		const FGameplayEffectContextHandle& EffectContext = ChangeData.GEModData->EffectSpec.GetEffectContext();
		return EffectContext.GetOriginalInstigator();
	}

	return nullptr;
}

void UMyLyraHealthComponent::HandleHealthChanged(const FOnAttributeChangeData& ChangeData)
{
	OnHealthChanged.Broadcast(this, ChangeData.OldValue, ChangeData.NewValue, GetInstigatorFromAttrChangeData(ChangeData));
}

UMyLyraHealthComponent* UMyLyraHealthComponent::FindHealthComponent(const AActor* InActor)
{
	if (InActor == nullptr)
	{
		return nullptr;
	}

	UMyLyraHealthComponent* HealthComponent = InActor->FindComponentByClass<UMyLyraHealthComponent>();
	return HealthComponent;
}

float UMyLyraHealthComponent::GetHealth() const
{
	return (HealthSet ? HealthSet->GetHealth() : 0.0f);
}

float UMyLyraHealthComponent::GetMaxhealth() const
{
	return (HealthSet ? HealthSet->GetMaxHealth() : 0.0f);
}

float UMyLyraHealthComponent::GetHealthNormalized() const
{
	if (HealthSet)
	{
		const float Health = HealthSet->GetHealth();
		const float MaxHealth = HealthSet->GetMaxHealth();
		return ((MaxHealth > 0.0f) ? (Health / MaxHealth) : 0.0f);
	}

	return 0.0f;
}
