// Fill out your copyright notice in the Description page of Project Settings.

#include "MyLyraCameraMode.h"

#include "MyLyraCameraComponent.h"
#include "MyLyraCameraMode_ThirdPerson.h"
#include "MyLyraPlayerCameraManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(MyLyraCameraMode)

FMyLyraCameraModeView::FMyLyraCameraModeView()
	: Location(ForceInit)
	, Rotation(ForceInit)
	, ControlRotation(ForceInit)
	, FieldOfView(MYLYRA_CAMERA_DEFAULT_FOV)
{
	
}

UMyLyraCameraMode::UMyLyraCameraMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	FieldOfView = MYLYRA_CAMERA_DEFAULT_FOV;
	ViewPitchMin = MYLYRA_CAMERA_DEFAULT_PITCH_MIN;
	ViewPitchMax = MYLYRA_CAMERA_DEFAULT_PITCH_MAX;

	BlendTime = 0.0f;
	BlendAlpha = 1.0f;
	BlendWeight = 1.0f;
}

void UMyLyraCameraMode::UpdateCameraMode(float DeltaTime)
{
	// Actor를 활용, Pivot[Location|Rotation] 을 계산해서, View를 업데이트
	UpdateView(DeltaTime);

	// BlendWeight를 DeltaTime을 활용해, BlendAlpha 계산 후, BlendFunction에 맞게 재매핑하여 최종 계산
	UpdateBlending(DeltaTime);
}

void UMyLyraCameraMode::UpdateView(float DeltaTime)
{
	// CameraMode를 가지고 있는 CameraComponent의 Owner인 Character(Pawn)을 활용, PivotLocation/Rotation을 반환
	FVector PivotLocation = GetPivotLocation();
	FRotator PivotRotation = GetPivotRotation();

	// Pitch 값에 대해 Min/Max를 Clamp
	PivotRotation.Pitch = FMath::ClampAngle(PivotRotation.Pitch, ViewPitchMin, ViewPitchMax);

	// FMyLyraCameraModeView에 PivotLocation / Rotation 설정
	View.Location = PivotLocation;
	View.Rotation = PivotRotation;

	// PivotRotation을 똑같이 ControlRotation으로 활용
	View.ControlRotation = View.Rotation;
	View.FieldOfView = FieldOfView;

	// 정리 : Character의 Location과 ControlRotation을 활용해, View를 업데이트!
}

void UMyLyraCameraMode::UpdateBlending(float DeltaTime)
{
	
}

UMyLyraCameraComponent* UMyLyraCameraMode::GetMyLyraCameraComponent() const
{
	// UMyLyraCameraMode 를 생성하는 곳은 UMyLyraCameraModeStack 이었음
	// - GetOuter() 를 MyLyraCameraMode로 MyLyraCameraComponent로 설정 하였음
	// - UMyLyraCameraModeStack::GetCameraModeInstance() 확인
	return CastChecked<UMyLyraCameraComponent>(GetOuter());
}

AActor* UMyLyraCameraMode::GetTargetActor() const
{
	const UMyLyraCameraComponent* MyLyraCameraComponent = GetMyLyraCameraComponent();
	return MyLyraCameraComponent->GetTargetActor();
}

FVector UMyLyraCameraMode::GetPivotLocation() const
{
	const AActor* TargetActor = GetTargetActor();
	check(TargetActor);

	if (const APawn* TargetPawn = Cast<APawn>(TargetActor))
	{
		// BaseEyeHeight를 고려해, ViewLocation을 반환
		return TargetPawn->GetPawnViewLocation();
	}

	return TargetActor->GetActorLocation();
}

FRotator UMyLyraCameraMode::GetPivotRotation() const
{
	const AActor* TargetActor = GetTargetActor();
	check(TargetActor);

	if (const APawn* TargetPawn = Cast<APawn>(TargetActor))
	{
		// 보통 Pawn의 ControlRotation을 반환
		return TargetPawn->GetViewRotation();
	}

	return TargetActor->GetActorRotation();
}

UMyLyraCameraModeStack::UMyLyraCameraModeStack(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

UMyLyraCameraMode* UMyLyraCameraModeStack::GetCameraModeInstance(TSubclassOf<UMyLyraCameraMode>& CameraModeClass)
{
	check(CameraModeClass);

	// CameraModeInstance에서 먼저 생성되어 있는지 확인 후, 반환
	for (UMyLyraCameraMode* CameraMode : CameraModeInstance)
	{
		// CameraMode는 UClass를 비교
		// - 즉, CameraMode는 클래스 타입에 하나만 생기게 됨
		if (IsValid(CameraMode) && (CameraMode->GetClass() == CameraModeClass))
		{
			return CameraMode;
		}
	}

	// CameraModeClass 에 알맞는 CameraMode의 인스턴스가 없다면 생성
	UMyLyraCameraMode* NewCameraMode = NewObject<UMyLyraCameraMode>(GetOuter(), CameraModeClass, NAME_None, RF_NoFlags);
	check(NewCameraMode);

	// 여기서 CameraModeInstance는 CameraModeClass에 맞는 인스턴스를 가지는(꽌리하는) 컨테이너라는 것을 알 수 있음
	CameraModeInstance.Add(NewCameraMode);

	return NewCameraMode;
}

void UMyLyraCameraModeStack::PushCameraMode(TSubclassOf<UMyLyraCameraMode>& CameraModeClass)
{
	if (IsValid(CameraModeClass) == false)
	{
		return;
	}

	UMyLyraCameraMode* CameraMode = GetCameraModeInstance(CameraModeClass);
	check(CameraMode);

	int32 StackSize = CameraModeStack.Num();
	if ((StackSize > 0) && (CameraModeStack[0] == CameraMode))
	{
		// CameraModeStack[0] 가장 최근에 이미 CameraMode가 Stacking 되었으므로 return!
		return;
	}

	// CameraModeStack에서 CameraMode에 맞는 Index를 찾는 변수
	int32 ExistingStackIndex = INDEX_NONE;

	// 위에서 아래로 최종 BlendWeight 값을 찾기 위한 변수로, 초기값으로 1.0f으로 설정
	float ExistingStackContribution = 1.0f;

	/**
	 * BlendWeight	|	ExistingStackContribution		|	ExistingStackContribution (accumulated)
	 * 0.1f			|	(1.0f) * 0.1f = 0.1f			|	(1.0f - 0.1f) = 0.9f
	 * 0.3f			|	(0.9f) * 0.3f = 0.27f			|	(1.0f - 0.3f) * 0.9f = 0.63f
	 * 0.6f			|	(0.63f) * 0.6f = 0.378f			|	(1.0f - 0.6f) * 0.63f = 0.252f
	 * 1.0f			|	(0.252f) * 1.0f = 0.252f		|	
	 *				|	0.1f + 0.27f + 0.378f + 0.252f = 1.0f
	 */
	for (int32 StackIndex = 0; StackIndex < StackSize; ++StackIndex)
	{
		if (CameraModeStack[StackIndex] == CameraMode)
		{
			ExistingStackIndex = StackIndex;
			// BlendWeight를 CameraMode에 추가
			// - 여기서 ExistingStackContribution 우리가 찾는 CameraMode를 찾았으니, 누적된 BlendWeight와 함꼐 BlendWeight을 곱해서, Loop 탈출
			ExistingStackContribution *= CameraMode->BlendWeight;
			break;
		}
		else
		{
			// 당연히 우리가 원하는 CaeramMode가 아니니, InvBlendWeight = (1.0f - BlendWeight) 를 곱해줘야, 값이 누적
			ExistingStackContribution *= (1.0f - CameraModeStack[StackIndex]->BlendWeight);
		}
	}

	// CameraMode 를 TOp으로 반영하기 위해, 당연히 중간에 있었다면, 제거하여 다시 Push 해줘야 함
	if (ExistingStackIndex != INDEX_NONE)
	{
		CameraModeStack.RemoveAt(ExistingStackIndex);
		--StackSize;
	}
	else
	{
		// 없었으니까 당연히 Contribution은 0으로 세팅
		ExistingStackContribution = 0.0f;
	}

	// BlendTime이 0보다 크다는 것을 Blend를 얼마의 시간동안 진행함을 의미. 따라서, ExistingStackContribution을 적용
	// - Blend 하지 않는다면, BlendWeight를 1.0f 을 넣어 새로 넣는 CameraMode만 적용
	const bool bShouldBlend = ((CameraMode->BlendTime > 0.0f) && (StackSize > 0));
	const float BlendWeight = (bShouldBlend ? ExistingStackContribution : 1.0f);
	CameraMode->BlendWeight = BlendWeight;
	
	CameraModeStack.Insert(CameraMode, 0);

	// 마지막은 항상 1.0f이 되어야 함
	CameraModeStack.Last()->BlendWeight = 1.0f;
}

void UMyLyraCameraModeStack::EvaluateStack(float DeltaTime, FMyLyraCameraModeView& OutCameraModeView)
{
	// Top -> Bottom [0 -> num] 까지 순차적으로 Stack에 있는 CameraMode 업데이트
	// 최신 -> 오래된
	UpdateStack(DeltaTime);

	// Bottom -> Top 까지 CameraModeView 에 대해 Blending 진행
	// 오래된 -> 최신
	BlendStack(OutCameraModeView);
}

void UMyLyraCameraModeStack::UpdateStack(float DeltaTime)
{
	const int32 StackSize = CameraModeStack.Num();
	if (StackSize <= 0)
	{
		return;
	}

	// CameraModeStack을 순회, CameraMode를 업데이트
	int32 RemoveCount = 0;
	int32 RemoveIndex = INDEX_NONE;
	for (int32 StackIndex = 0; StackIndex < StackSize; ++StackIndex)
	{
		UMyLyraCameraMode* CameraMode = CameraModeStack[StackIndex];
		check(CameraMode);
		
		CameraMode->UpdateCameraMode(DeltaTime);

		// 하나라도 CameraMode의 BlendWeight가 1.0f에 도달하면, 그 이후의 CameraMode를 제거
		// CameraMode는 최소 한 개를 유지
		if (CameraMode->BlendWeight >= 1.0f)
		{
			RemoveIndex = (StackIndex + 1);
			RemoveCount = (StackSize - RemoveIndex);
			break;
		}
	}

	if (RemoveCount > 0)
	{
		CameraModeStack.RemoveAt(RemoveIndex, RemoveCount);
	}
}

void UMyLyraCameraModeStack::BlendStack(FMyLyraCameraModeView& OutCameraModeView) const
{
}
