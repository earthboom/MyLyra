// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "MyLyraCameraMode.generated.h"

class UMyLyraCameraComponent;

// 현재 카메라가 바라보는 시야 자치 (렌더링 되어야 하는 영역)
struct FMyLyraCameraModeView
{
	FMyLyraCameraModeView();

	FVector Location = FVector::ZeroVector;
	FRotator Rotation = FRotator::ZeroRotator;
	FRotator ControlRotation = FRotator::ZeroRotator;
	float FieldOfView = 0.0f;
};

// Camera Blending 대상 유닛
UCLASS(Abstract, NotBlueprintable)
class MYLYRA_API UMyLyraCameraMode : public UObject
{
	GENERATED_BODY()

public:
	UMyLyraCameraMode(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	void UpdateCameraMode(float DeltaTime);
	virtual void UpdateView(float DeltaTime);
	void UpdateBlending(float DeltaTime);

	UMyLyraCameraComponent* GetMyLyraCameraComponent() const;
	AActor* GetTargetActor() const;
	
	FVector GetPivotLocation() const;
	FRotator GetPivotRotation() const;

	// CameramMode에 의해 생성된 CameraModeView
	FMyLyraCameraModeView View;

	UPROPERTY(EditDefaultsOnly, Category = "View", Meta = (UIMin = "5.0", UIMax = "170", CLampMin = "5.0", ClampMax = "170.0"))
	float FieldOfView = 0.0f;

	UPROPERTY(EditDefaultsOnly, Category = "View", Meta = (UIMin = "-89.9", UIMax = "89.9", CLampMin = "-89.9", ClampMax = "89.9"))
	float ViewPitchMin = 0.0f;

	UPROPERTY(EditDefaultsOnly, Category = "View", Meta = (UIMin = "-89.9", UIMax = "89.9", CLampMin = "-89.9", ClampMax = "89.9"))
	float ViewPitchMax = 0.0f;
	
	// 얼마 동안 Blend를 진행할지 시간을 의미
	UPROPERTY(EditDefaultsOnly, Category = "Blending")
	float BlendTime = 0.0f;

	// 선형적인 Blend rkqt [0, 1]
	float BlendAlpha = 0.0f;

	/*
	 * 해당 CameraMode의 최종 Blend 값
	 * 앞서 BlendAlpha의 선형 값을 매핑하여 최종 BlendWeight를 계산
	 */
	float BlendWeight = 0.0f;
};

// Camera Blending을 담당하는 객체
UCLASS()
class UMyLyraCameraModeStack : public UObject
{
	GENERATED_BODY()

public:
	UMyLyraCameraModeStack(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UMyLyraCameraMode* GetCameraModeInstance(TSubclassOf<UMyLyraCameraMode>& CameraModeClass);
	void PushCameraMode(TSubclassOf<UMyLyraCameraMode>& CameraModeClass);

	void EvaluateStack(float DeltaTime, FMyLyraCameraModeView& OutCameraModeView);
	void UpdateStack(float DeltaTime);
	void BlendStack(FMyLyraCameraModeView& OutCameraModeView) const;
	
	// 생성된 CameraMode 관리
	UPROPERTY()
	TArray<TObjectPtr<UMyLyraCameraMode>> CameraModeInstance;

	// Camera Matrix Blend 업데이트 진행 큐
	UPROPERTY()
	TArray<TObjectPtr<UMyLyraCameraMode>> CameraModeStack;
};
