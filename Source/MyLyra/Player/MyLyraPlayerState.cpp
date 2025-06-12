// Fill out your copyright notice in the Description page of Project Settings.


#include "MyLyraPlayerState.h"

#include "MyLyra/GameModes/MyLyraExperienceManagerComponent.h"
#include "MyLyra/GameModes/MyLyraGameModeBase.h"

void AMyLyraPlayerState::PostInitializeComponents()
{
	Super::PostInitializeComponents();

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
	check(PawnData == nullptr);	// PawnData가 두번 설정되는 것을 원치 않음
	PawnData = InPawnData;
}
