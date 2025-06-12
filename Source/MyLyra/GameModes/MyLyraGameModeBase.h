// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MyLyraGameModeBase.generated.h"

class UMyLyraPawnData;
class UMyLyraExperienceDefinition;
/**
 * 
 */
UCLASS()
class MYLYRA_API AMyLyraGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	AMyLyraGameModeBase();

	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	virtual void InitGameState() final;

	// GetDefaultPawnClassForController
	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;
	
	// HandleStartingNewPlayer
	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;

	// SpawnDefaultPawnAtTransform
	virtual APawn* SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer, const FTransform& SpawnTransform) override;
	
	void HandleMatchAssignmentIfNotExpectingOne();
	void OnMatchAssignmentGiven(FPrimaryAssetId ExperienceId);
	bool IsExperienceLoaded() const;
	void OnExperienceLoaded(const UMyLyraExperienceDefinition* CurrentExperience);
	const UMyLyraPawnData* GetPawnDataForController(const AController* InController) const;
};
