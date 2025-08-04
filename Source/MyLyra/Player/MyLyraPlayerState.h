// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "MyLyraPlayerState.generated.h"

class UMyLyraAbilitySystemComponent;
class UMyLyraPawnData;
class UMyLyraExperienceDefinition;
/**
 * 
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
