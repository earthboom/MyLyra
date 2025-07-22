// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyLyraCharacterPartTypes.h"
#include "Components/ControllerComponent.h"
#include "MyLyraControllerComponent_CharacterParts.generated.h"

/** ControllerComponent 가 소유하는 Character Parts */
USTRUCT()
struct FMyLyraControllerCharacterPartEntry
{
	GENERATED_BODY()

	/** Character Part에 대한 정의 (meta data) */
	UPROPERTY(EditAnywhere)
	FMyLyraCharacterPart Part;

	/** Character Part Handle (고유값)
	 * - Controller가 Possess하고 있는 Pawn에서 생성한(Instance) Character Part 핸들값
	 */
	FMyLyraCharacterPartHandle Handle;
};

/**
 * 
 */
UCLASS(meta = (BlueprintSpawnableComponent))
class MYLYRA_API UMyLyraControllerComponent_CharacterParts : public UControllerComponent
{
	GENERATED_BODY()
	
public:
	UMyLyraControllerComponent_CharacterParts(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UPROPERTY(EditAnywhere, Category = Cosmetics)
	TArray<FMyLyraControllerCharacterPartEntry> CharacterParts;
};
