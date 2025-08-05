// Fill out your copyright notice in the Description page of Project Settings.

#include "MyLyraPlayerController.h"
#include "MyLyraPlayerState.h"
#include "MyLyra/AbilitySystem/MyLyraAbilitySystemComponent.h"
#include "MyLyra/Camera/MyLyraPlayerCameraManager.h"

AMyLyraPlayerController::AMyLyraPlayerController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PlayerCameraManagerClass = AMyLyraPlayerCameraManager::StaticClass();
}

void AMyLyraPlayerController::PostProcessInput(const float DeltaTime, const bool bGamePaused)
{
	// PostProcessInput이 언제 호출되는지 확인
	// - UPlayerInput::ProcessInputStack() 에서 호출 됨

	UMyLyraAbilitySystemComponent* MyLyraASC = GetMyLyraAbilitySystemComponent();
	if (IsValid(MyLyraASC))
	{
		MyLyraASC->ProcessAbilityInput(DeltaTime, bGamePaused);
	}

	Super::PostProcessInput(DeltaTime, bGamePaused);
}

AMyLyraPlayerState* AMyLyraPlayerController::GetMyLyraPlayerState() const
{
	// ECastCheckedType의 NullAllowed는 Null반환을 의도할 경우 유용
	return CastChecked<AMyLyraPlayerState>(PlayerState, ECastCheckedType::NullAllowed);
}

UMyLyraAbilitySystemComponent* AMyLyraPlayerController::GetMyLyraAbilitySystemComponent() const
{
	const AMyLyraPlayerState* MyLyraPS = GetMyLyraPlayerState();
	return (MyLyraPS ? MyLyraPS->GetMyLyraAbilitySystemComponent() : nullptr);
}
