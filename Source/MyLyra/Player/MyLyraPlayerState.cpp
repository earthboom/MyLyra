// Fill out your copyright notice in the Description page of Project Settings.

#include "MyLyraPlayerState.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "MyLyra/AbilitySystem/MyLyraAbilitySet.h"
#include "MyLyra/AbilitySystem/MyLyraAbilitySystemComponent.h"
#include "MyLyra/Character/MyLyraPawnData.h"
#include "MyLyra/GameModes/MyLyraExperienceManagerComponent.h"
#include "MyLyra/GameModes/MyLyraGameModeBase.h"

AMyLyraPlayerState::AMyLyraPlayerState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AbilitySystemComponent = ObjectInitializer.CreateDefaultSubobject<UMyLyraAbilitySystemComponent>(this, TEXT("AbilitySystemComponent"));
}

void AMyLyraPlayerState::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	check(IsValid(AbilitySystemComponent));
	// 아래의 코드는 우리가 InitAbilityActorInfo를 재호출을 통하는 이유를 설명하는 코드
	{
		// 처음 InitAbilityActorInfo를 호출 당시, OwnerActor와 AvatarActor가 같은 Actor를 가르키고 있으며, 이는 PlayerState
		// - OwnerActor는 PlayerState가 의도하는게 맞으나, AvatarActor는 PlayerController가 소유하는 대상인 Pawn이어야 함
		// - 이를 위해 다시 세팅
		FGameplayAbilityActorInfo* ActorInfo = AbilitySystemComponent->AbilityActorInfo.Get();
		check(ActorInfo->OwnerActor == this);
		check(ActorInfo->OwnerActor == ActorInfo->AvatarActor);
	}
	AbilitySystemComponent->InitAbilityActorInfo(this, GetPawn());

	const AGameStateBase* GameState = GetWorld()->GetGameState();
	check(GameState);

	UMyLyraExperienceManagerComponent* ExperienceManagerComponent = GameState->FindComponentByClass<UMyLyraExperienceManagerComponent>();
	check(ExperienceManagerComponent);

	ExperienceManagerComponent->CallOrRegister_OnExperienceLoaded(FOnMyLyraExperienceLoaded::FDelegate::CreateUObject(this, &AMyLyraPlayerState::OnExperienceLoaded));
}

void AMyLyraPlayerState::OnExperienceLoaded(const UMyLyraExperienceDefinition* CurrentExperience)
{
	if (AMyLyraGameModeBase* GameMode = GetWorld()->GetAuthGameMode<AMyLyraGameModeBase>())
	{
		const UMyLyraPawnData* NewPawnData = GameMode->GetPawnDataForController(GetOwningController());
		check(NewPawnData);

		SetPawnData(NewPawnData);
	}
}

void AMyLyraPlayerState::SetPawnData(const UMyLyraPawnData* InPawnData)
{
	check(InPawnData);
	check(IsValid(PawnData) == false); // PawnData가 두번 설정되는 것을 원치 않음

	PawnData = InPawnData;

	// PawnData의 AbilitySet을 순회하며, ASC에 Ability를 할당(Give)
	// - 이 과정에서 ASC의 ActivatableAbilities에 추가됨
	for (UMyLyraAbilitySet* AbilitySet : PawnData->AbilitySets)
	{
		if (IsValid(AbilitySet))
		{
			AbilitySet->GiveToAbilitySystem(AbilitySystemComponent, nullptr);
		}
	}
}
