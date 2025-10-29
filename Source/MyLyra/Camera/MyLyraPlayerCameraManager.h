// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "MyLyraPlayerCameraManager.generated.h"

// Controller 에 바인딩 된 CameraManager
#define MYLYRA_CAMERA_DEFAULT_FOV (80.0f)
#define MYLYRA_CAMERA_DEFAULT_PITCH_MIN (-89.0f)
#define MYLYRA_CAMERA_DEFAULT_PITCH_MAX (89.0f)

/**
 * PlayerController는 CameraManager에 대한 소유권을 가진다.
 */
UCLASS()
class MYLYRA_API AMyLyraPlayerCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()

public:
	AMyLyraPlayerCameraManager(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};
