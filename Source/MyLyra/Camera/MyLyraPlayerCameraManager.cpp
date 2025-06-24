// Fill out your copyright notice in the Description page of Project Settings.


#include "MyLyraPlayerCameraManager.h"

AMyLyraPlayerCameraManager::AMyLyraPlayerCameraManager(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DefaultFOV = MYLYRA_CAMERA_DEFAULT_FOV;
	ViewPitchMin = MYLYRA_CAMERA_DEFAULT_PITCH_MIN;
	ViewPitchMax = MYLYRA_CAMERA_DEFAULT_PITCH_MAX;
}
