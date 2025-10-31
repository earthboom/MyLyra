// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "MyLyraPlayerState.generated.h"

class UMyLyraAbilitySystemComponent;
class UMyLyraPawnData;
class UMyLyraExperienceDefinition;

/**
 * 2025.10.31
 * - AMyLyraPlayerState는 MyLyraPawnData가 필요 (PawnData)
 *		- 당연히 PlayerController가 Possess할 Character Spawn에 대한 정보가 필요하기 때문
 *		- MyLyraPawnData는 ExperienceDefinition에 있다!
 */
UCLASS()
class MYLYRA_API AMyLyraPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	AMyLyraPlayerState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// AActor's interface
	virtual void PostInitializeComponents() final;

	// member methods
	template <class T>
	const T* GetPawnData() const { return Cast<T>(PawnData); }

	void OnExperienceLoaded(const UMyLyraExperienceDefinition* CurrentExperience);
	void SetPawnData(const UMyLyraPawnData* InPawnData);
	UMyLyraAbilitySystemComponent* GetMyLyraAbilitySystemComponent() const { return AbilitySystemComponent; }

	UPROPERTY()
	TObjectPtr<const UMyLyraPawnData> PawnData;

	UPROPERTY(VisibleAnywhere, Category = "MyLyra|PlayerState")
	TObjectPtr<UMyLyraAbilitySystemComponent> AbilitySystemComponent;
};
