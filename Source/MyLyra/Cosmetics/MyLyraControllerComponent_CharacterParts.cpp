// Fill out your copyright notice in the Description page of Project Settings.

#include "MyLyraControllerComponent_CharacterParts.h"
#include "MyLyraPawnComponent_CharacterParts.h"

UMyLyraControllerComponent_CharacterParts::UMyLyraControllerComponent_CharacterParts(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UMyLyraControllerComponent_CharacterParts::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		AController* OwningController = GetController<AController>();
		if (IsValid(OwningController))
		{
			OwningController->OnPossessedPawnChanged.AddDynamic(this, &ThisClass::OnPossessedPawnChanged);
		}
	}
}

void UMyLyraControllerComponent_CharacterParts::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	RemoveAllCharacterParts();
	
	Super::EndPlay(EndPlayReason);
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

void UMyLyraControllerComponent_CharacterParts::RemoveAllCharacterParts()
{
	UMyLyraPawnComponent_CharacterParts* PawnCustomizer = GetPawnCustomizer();
	if (IsValid(PawnCustomizer))
	{
		for (FMyLyraControllerCharacterPartEntry& Entry : CharacterParts)
		{
			PawnCustomizer->RemoveCharacterPart(Entry.Handle);
		}
	}
	CharacterParts.Reset();
}

void UMyLyraControllerComponent_CharacterParts::OnPossessedPawnChanged(APawn* OldPawn, APawn* NewPawn)
{
	// 이젠 OldPawn에 대해서는 Character Parts를 전부 제거
	UMyLyraPawnComponent_CharacterParts* OldCustomizer = OldPawn ? OldPawn->FindComponentByClass<UMyLyraPawnComponent_CharacterParts>() : nullptr;
	if (IsValid(OldCustomizer))
	{
		for (FMyLyraControllerCharacterPartEntry& Entry : CharacterParts)
		{
			OldCustomizer->RemoveCharacterPart(Entry.Handle);
			Entry.Handle.Reset();
		}
	}

	// 새로운 Pawn에 대해 기존 Controller가 가지고 있는 Character Parts를 추가
	UMyLyraPawnComponent_CharacterParts* NewCustomizer = NewPawn ? NewPawn->FindComponentByClass<UMyLyraPawnComponent_CharacterParts>() : nullptr;
	if (IsValid(NewCustomizer))
	{
		for (FMyLyraControllerCharacterPartEntry& Entry : CharacterParts)
		{
			check(Entry.Handle.IsValid() == false);
			Entry.Handle = NewCustomizer->AddCharacterPart(Entry.Part);
		}
	}
}
