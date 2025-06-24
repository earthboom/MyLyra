// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyLyraCameraMode.h"
#include "Camera/CameraComponent.h"
#include "MyLyraCameraComponent.generated.h"

class UMyLyraCameraModeStack;

template<class TClass> class TSubclassOf;

DECLARE_DELEGATE_RetVal(TSubclassOf<UMyLyraCameraMode>, FMyLyraCameraModeDelegate);

UCLASS()
class MYLYRA_API UMyLyraCameraComponent : public UCameraComponent
{
	GENERATED_BODY()

public:
	UMyLyraCameraComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	static UMyLyraCameraComponent* FindCameraComponent(const AActor* Actor){ return (IsValid(Actor) ? Actor->FindComponentByClass<UMyLyraCameraComponent>() : nullptr); }

	// CameraComponent Interface
	virtual void OnRegister() final;
	virtual void GetCameraView(float DeltaTime, FMinimalViewInfo& DesiredView) override;

	void UpdateCameraModes();

	// Camera의 Blending 기능을 지원하는 stack
	UPROPERTY()
	TObjectPtr<UMyLyraCameraModeStack> CameraModeStack;

	// 현재 CameraMode를 가져오는 Delegate
	FMyLyraCameraModeDelegate DetermineCameraModeDelegate;
};
