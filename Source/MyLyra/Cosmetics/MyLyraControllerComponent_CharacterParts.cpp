// Fill out your copyright notice in the Description page of Project Settings.

#include "MyLyraControllerComponent_CharacterParts.h"
#include "MyLyraPawnComponent_CharacterParts.h"

UMyLyraControllerComponent_CharacterParts::UMyLyraControllerComponent_CharacterParts(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

PRAGMA_DISABLE_OPTIMIZATION
UMyLyraPawnComponent_CharacterParts* UMyLyraControllerComponent_CharacterParts::GetPawnCustomizer() const
{
	APawn* ControlledPawn = GetPawn<APawn>();
	if (IsValid(ControlledPawn))
	{
		/**
		 * 앞서 MyLyraPawnComponent_CharacterParts를 상속받는 BP_MannequinPawnCosmetics를 이미 BP_Hero_ShooterMannequin에 추가함
		 * BP_MannequinPawnCosmetics를 반환되길 기대
		 */
		return ControlledPawn->FindComponentByClass<UMyLyraPawnComponent_CharacterParts>();
	}

	return nullptr;
}
PRAGMA_ENABLE_OPTIMIZATION

void UMyLyraControllerComponent_CharacterParts::AddCharacterPart(const FMyLyraCharacterPart& NewPart)
{
	AddCharacterPartInternal(NewPart);
}

void UMyLyraControllerComponent_CharacterParts::AddCharacterPartInternal(const FMyLyraCharacterPart& NewPart)
{
	FMyLyraControllerCharacterPartEntry& NewEntry = CharacterParts.AddDefaulted_GetRef();
	NewEntry.Part = NewPart;

	UMyLyraPawnComponent_CharacterParts* PawnCustomizer = GetPawnCustomizer();
	if (IsValid(PawnCustomizer))
	{
		NewEntry.Handle = PawnCustomizer->AddCharacterPart(NewPart);
	}
}
