// Fill out your copyright notice in the Description page of Project Settings.

#include "MyLyraCameraComponent.h"
#include "MyLyraCameraMode.h"

UMyLyraCameraComponent::UMyLyraCameraComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, CameraModeStack(nullptr)
{
}

void UMyLyraCameraComponent::OnRegister()
{
	Super::OnRegister();
	
	if (IsValid(CameraModeStack) == false)
	{
		// 초기화(BeginPlay 같은)가 딱히 필요없는 객체로 NewObject로 할당
		CameraModeStack = NewObject<UMyLyraCameraModeStack>(this);
	}
}

// 매 프래임 호출되는 함수
void UMyLyraCameraComponent::GetCameraView(float DeltaTime, FMinimalViewInfo& DesiredView)
{
	check(CameraModeStack);
	
	UpdateCameraModes();
}

void UMyLyraCameraComponent::UpdateCameraModes()
{
	check(CameraModeStack);

	// DetermineCameraModeDelegate 는 CameraMode Class를 반환
	// - 해당 Delegate는 HeroComponent 의 멤버 함수로 바인딩 됨
	if (DetermineCameraModeDelegate.IsBound())
	{
		const TSubclassOf<UMyLyraCameraMode> CameraMode = DetermineCameraModeDelegate.Execute();
		if (IsValid(CameraMode))
		{
			// CameraModeStack->PushCameraMode(CameraMode);
		}
	}
}
