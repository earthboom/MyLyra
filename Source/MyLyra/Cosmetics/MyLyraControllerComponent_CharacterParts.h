// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyLyraCharacterPartTypes.h"
#include "Components/ControllerComponent.h"
#include "MyLyraControllerComponent_CharacterParts.generated.h"

class UMyLyraPawnComponent_CharacterParts;

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

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	UMyLyraPawnComponent_CharacterParts* GetPawnCustomizer() const;

	UFUNCTION(BlueprintCallable, Category = Cosmetics)
	void AddCharacterPart(const FMyLyraCharacterPart& NewPart);

	void AddCharacterPartInternal(const FMyLyraCharacterPart& NewPart);

	void RemoveAllCharacterParts();

	UFUNCTION()
	void OnPossessedPawnChanged(APawn* OldPawn, APawn* NewPawn);

	UPROPERTY(EditAnywhere, Category = Cosmetics)
	TArray<FMyLyraControllerCharacterPartEntry> CharacterParts;
};
