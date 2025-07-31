// Fill out your copyright notice in the Description page of Project Settings.

#include "MyLyraGameModeBase.h"
#include "MyLyraExperienceManagerComponent.h"
#include "MyLyraGameState.h"
#include "MyLyraExperienceDefinition.h"
#include "Kismet/GameplayStatics.h"
#include "MyLyra/MyLyraLogChannels.h"
#include "MyLyra/Character/MyLyraCharacter.h"
#include "MyLyra/Character/MyLyraPawnData.h"
#include "MyLyra/Character/MyLyraPawnExtensionComponent.h"
#include "MyLyra/Player/MyLyraPlayerController.h"
#include "MyLyra/Player/MyLyraPlayerState.h"

AMyLyraGameModeBase::AMyLyraGameModeBase()
{
	GameStateClass = AMyLyraGameState::StaticClass();
	PlayerControllerClass = AMyLyraPlayerController::StaticClass();
	PlayerStateClass = AMyLyraPlayerState::StaticClass();
	DefaultPawnClass = AMyLyraCharacter::StaticClass();
}

void AMyLyraGameModeBase::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	// 아직 GameInstance를 통해, 초기화 작업이 진행되므로, 현 프레임에는 Lyra의 concept인 EXPERIENCE 처리를 진행할 수 없음
	// 이를 처리하기 위해, 프레임 뒤에 이벤트를 받아 처리를 이어서 진행
	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &AMyLyraGameModeBase::HandleMatchAssignmentIfNotExpectingOne);
}

void AMyLyraGameModeBase::InitGameState()
{
	Super::InitGameState();

	// Experience 비동기 로딩을 위한 delegate 준비
	UMyLyraExperienceManagerComponent* ExperienceManagerComponent = GameState->FindComponentByClass<UMyLyraExperienceManagerComponent>();
	check(ExperienceManagerComponent);

	// OnExperienceLoaded 등록
	ExperienceManagerComponent->CallOrRegister_OnExperienceLoaded(FOnMyLyraExperienceLoaded::FDelegate::CreateUObject(this, &AMyLyraGameModeBase::OnExperienceLoaded));
}

UClass* AMyLyraGameModeBase::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	// GetPawnDataForController를 활용해, PawnData로부터 PawnClass를 유도
	if (const UMyLyraPawnData* PawnData = GetPawnDataForController(InController))
	{
		if (PawnData->PawnClass)
		{
			return PawnData->PawnClass;
		}
	}

	return Super::GetDefaultPawnClassForController_Implementation(InController);
}

void AMyLyraGameModeBase::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	if (IsExperienceLoaded())
	{
		Super::HandleStartingNewPlayer_Implementation(NewPlayer);
	}
}

APawn* AMyLyraGameModeBase::SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer, const FTransform& SpawnTransform)
{
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.Instigator = GetInstigator();
	SpawnInfo.ObjectFlags |= RF_Transient;
	SpawnInfo.bDeferConstruction = true;

	if (UClass* PawnClass = GetDefaultPawnClassForController(NewPlayer))
	{
		if (APawn* SpawnedPawn = GetWorld()->SpawnActor<APawn>(PawnClass, SpawnTransform, SpawnInfo))
		{
			// FindPawnExtensionComponent 구성
			if (UMyLyraPawnExtensionComponent* PawnExtensionComponent = UMyLyraPawnExtensionComponent::FindPawnExtensionComponent(SpawnedPawn))
			{
				if (const UMyLyraPawnData* PawnData = GetPawnDataForController(NewPlayer))
				{
					PawnExtensionComponent->SetPawnData(PawnData);
				}
			}
			SpawnedPawn->FinishSpawning(SpawnTransform);
			return SpawnedPawn;
		}
	}

	return nullptr;
}

void AMyLyraGameModeBase::HandleMatchAssignmentIfNotExpectingOne()
{
	// 로딩할 Experience 에 대해 PrimaryAssetId를 생성하여, OnMatchAssignmentGiven으로 넘겨줌
	FPrimaryAssetId ExperienceId;

	// Precedence order (highest wins)
	// - matchmaking assignment (if present)
	// - default experience

	UWorld* World = GetWorld();

	// URL 과 함께 ExtraArgs로 넘겼던 정보는 OptionString에 저장되어 있음
	if (ExperienceId.IsValid() == false && UGameplayStatics::HasOption(OptionsString, TEXT("Experience")))
	{
		// Experience의 Value를 가져와서, PrimaryAssetid를 생성. 이때, MyLyraExperienceDefinition의 Class 이름을 사용
		const FString ExperienceFromOptions = UGameplayStatics::ParseOption(OptionsString, TEXT("Experience"));
		ExperienceId = FPrimaryAssetId(FPrimaryAssetType(UMyLyraExperienceDefinition::StaticClass()->GetFName()), FName(*ExperienceFromOptions));
	}

	// fall back to the default experience
	// 일단 기본 옵션으로 Default한 설정
	if (ExperienceId.IsValid() == false)
	{
		ExperienceId = FPrimaryAssetId(FPrimaryAssetType("MyLyraExperienceDefinition"), FName("BP_MyLyraDefaultExperience"));
	}

	OnMatchAssignmentGiven(ExperienceId);
}

void AMyLyraGameModeBase::OnMatchAssignmentGiven(FPrimaryAssetId ExperienceId)
{
	// ExperienceManagerComponent를 활용해 Experience를 로딩하기 위해, ExperienceManagerComponent의 ServerSetCurrentExperience를 호출
	check(ExperienceId.IsValid());

	UMyLyraExperienceManagerComponent* ExperienceManagerComponent = GameState->FindComponentByClass<UMyLyraExperienceManagerComponent>();
	check(ExperienceManagerComponent);
	ExperienceManagerComponent->ServerSetCurrentExperience(ExperienceId);
}

bool AMyLyraGameModeBase::IsExperienceLoaded() const
{
	check(GameState);
	UMyLyraExperienceManagerComponent* ExperienceManagerComponent = GameState->GetComponentByClass<UMyLyraExperienceManagerComponent>();
	check(ExperienceManagerComponent);

	return ExperienceManagerComponent->IsExperienceLoaded();
}

void AMyLyraGameModeBase::OnExperienceLoaded(const UMyLyraExperienceDefinition* CurrentExperience)
{
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* PC = Cast<APlayerController>(*Iterator);

		// PlayerController가 Pawn을 Possess하지 않았다면, RestartPlayer를 통해 Pawn을 다시 Spawn
		if (PC && PC->GetPawn() == nullptr)
		{
			if (PlayerCanRestart(PC))
			{
				RestartPlayer(PC);
			}
		}
	}
}

const UMyLyraPawnData* AMyLyraGameModeBase::GetPawnDataForController(const AController* InController) const
{
	// 게임 도중 PawnData가 오버라이드 되었을 경우, PawnData는 PlayerState에서 가져오게 됨
	if (IsValid(InController))
	{
		if (const AMyLyraPlayerState* MyLyraPS = InController->GetPlayerState<AMyLyraPlayerState>())
		{
			// GetPawnData 구현
			if (const UMyLyraPawnData* PawnData = MyLyraPS->GetPawnData<UMyLyraPawnData>())
			{
				return PawnData;
			}
		}
	}

	// fall back to the default for the current experience
	// 아직 PlayerState에 PawnData가 설정되어 있지 않은 경우, ExperienceManagerComponent와 CurrentExperience로부터 가져와서 설정
	check(GameState);
	UMyLyraExperienceManagerComponent* ExperienceManagerComponent = GameState->FindComponentByClass<UMyLyraExperienceManagerComponent>();
	check(ExperienceManagerComponent);

	if (ExperienceManagerComponent->IsExperienceLoaded())
	{
		const UMyLyraExperienceDefinition* Experience = ExperienceManagerComponent->GetCurrentExperienceChecked();
		if (Experience->DefaultPawnData)
		{
			return Experience->DefaultPawnData;
		}
	}

	return nullptr;
}
