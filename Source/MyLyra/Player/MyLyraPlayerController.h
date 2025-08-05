// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ModularPlayerController.h"
#include "MyLyraPlayerController.generated.h"

class UMyLyraAbilitySystemComponent;
class AMyLyraPlayerState;
/**
 * 
 */
UCLASS()
class MYLYRA_API AMyLyraPlayerController : public AModularPlayerController
{
	GENERATED_BODY()

public:
	AMyLyraPlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** PlayerController Interface */
	virtual void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;

	AMyLyraPlayerState* GetMyLyraPlayerState() const;
	UMyLyraAbilitySystemComponent* GetMyLyraAbilitySystemComponent() const;
};
