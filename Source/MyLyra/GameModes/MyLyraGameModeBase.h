// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MyLyraGameModeBase.generated.h"

class UMyLyraPawnData;
class UMyLyraExperienceDefinition;
/**
 * GameMode
 * - 보통 World와 GameState와 각각 1:1:1 대응을 맺는다.
 * - 게임의 Rule을 관장하는 객체라고 이해할 것 ( 플레이어 소환(PlayerController, PlayerState))
 */
UCLASS()
class MYLYRA_API AMyLyraGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	AMyLyraGameModeBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) final;
	virtual void InitGameState() final;

	/** GetDefaultPawnClassForController */
	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) final; // (20)

	/** HandleStartingNewPlayer */
	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) final;

	/** SpawnDefaultPawnAtTransform */
	virtual APawn* SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer, const FTransform& SpawnTransform) final;

	/**
	 * InitNewPlayer의 로직 흐름을 우리에 의도대로 수정하기 위해 따로 구현하여 우리가 원하는 순서의 흐름으로 만들어줌
	 */
	// virtual AActor* FindPlayerStart_Implementation(AController* Player, const FString& IncomingName = L"") override; (18)
	// virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override; (19)

	void HandleMatchAssignmentIfNotExpectingOne();
	void OnMatchAssignmentGiven(FPrimaryAssetId ExperienceId);
	bool IsExperienceLoaded() const;
	void OnExperienceLoaded(const UMyLyraExperienceDefinition* CurrentExperience);
	const UMyLyraPawnData* GetPawnDataForController(const AController* InController) const; // (21)
};
