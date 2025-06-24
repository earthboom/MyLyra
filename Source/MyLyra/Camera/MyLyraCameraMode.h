// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "MyLyraCameraMode.generated.h"

// Camera Blending 대상 유닛
UCLASS(Abstract, NotBlueprintable)
class MYLYRA_API UMyLyraCameraMode : public UObject
{
	GENERATED_BODY()

public:
	UMyLyraCameraMode(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};

// Camera Blending을 담당하는 객체
UCLASS()
class UMyLyraCameraModeStack : public UObject
{
	GENERATED_BODY()

public:
	UMyLyraCameraModeStack(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// 생성된 CameraMode 관리
	UPROPERTY()
	TArray<TObjectPtr<UMyLyraCameraMode>> CameraModeInstance;

	// Camera Matrix Blend 업데이트 진행 큐
	UPROPERTY()
	TArray<TObjectPtr<UMyLyraCameraMode>> CameraModeStack;
};
