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

	// 해당 함수는 HeroComponent에서 PawnData 에서 DefaultCameraMode를 가져와서 CameraModeStack에 추가하여, CameraMode 블랜딩을 준비
	UpdateCameraModes();

	// EvaluateStack은 CameraModeStack에 있는 CameraMode를 업데이트(+블랜딩)하고 CameraModeStack을 Bottom-Top까지 업데이트된 CameraMode들을 Lerp를 진행
	// - 이에 대한 결과는 CameraModeView에 캐싱됨
	FMyLyraCameraModeView CameraModeView;
	CameraModeStack->EvaluateStack(DeltaTime, CameraModeView);

	APawn* TargetPawn = Cast<APawn>(GetTargetActor());
	if (IsValid(TargetPawn))
	{
		APlayerController* PC = TargetPawn->GetController<APlayerController>();
		if (IsValid(PC))
		{
			// PlayerController의 ControlRotation을 계산된 CameraModeView의 ControlRotation으로 업데이트
			// - 해당 함수는 PC가 Possess하고 있는 Pawn의 RootComponent에 ControlRotation을 반영함 ( == 조정하고 있는 캐릭터에 회전을 적용)
			PC->SetControlRotation(CameraModeView.ControlRotation);
		}
	}

	// Camera의 Location와 Rotation 반영
	// - SetWorldLocationAndRotation -> UpdateChildTransform 분석 (SceneGraph 관계 업데이트 이해 가능)
	// 정리 : CameraComponent에 대해 Parent의 SceneGraph 관계를 고려하여 업데이트 진행
	SetWorldLocationAndRotation(CameraModeView.Location, CameraModeView.Rotation);

	// FOV 업데이트
	FieldOfView = CameraModeView.FieldOfView;

	/**
	 * ControlRotation vs Rotation 차이
	 * - ControlRotation은 PC가 조정하는 Pawn의 Rotation을 적용하는 값
	 * - Roatation은 Camera에 반영하는 Rotation
	 */

	// FMinimalViewInfo 업데이트
	// - CameraComponent의 변화 사항을 반영해서 최종 렌더링까지 반영하게 됨
	DesiredView.Location = CameraModeView.Location;
	DesiredView.Rotation = CameraModeView.Rotation;
	DesiredView.FOV = CameraModeView.FieldOfView;
	DesiredView.OrthoWidth = OrthoWidth;
	DesiredView.OrthoNearClipPlane = OrthoNearClipPlane;;
	DesiredView.OrthoFarClipPlane = OrthoFarClipPlane;
	DesiredView.AspectRatio = AspectRatio;
	DesiredView.bConstrainAspectRatio = bConstrainAspectRatio;
	DesiredView.bUseFieldOfViewForLOD = bUseFieldOfViewForLOD;
	DesiredView.ProjectionMode = ProjectionMode;
	DesiredView.PostProcessBlendWeight = PostProcessBlendWeight;

	if (PostProcessBlendWeight > 0.0f)
	{
		DesiredView.PostProcessSettings = PostProcessSettings;
	}
}

void UMyLyraCameraComponent::UpdateCameraModes()
{
	check(CameraModeStack);

	// DetermineCameraModeDelegate 는 CameraMode Class를 반환
	// - 해당 Delegate는 HeroComponent 의 멤버 함수로 바인딩 됨
	if (DetermineCameraModeDelegate.IsBound())
	{
		TSubclassOf<UMyLyraCameraMode> CameraMode = DetermineCameraModeDelegate.Execute();
		if (IsValid(CameraMode))
		{
			CameraModeStack->PushCameraMode(CameraMode);
		}
	}
}
