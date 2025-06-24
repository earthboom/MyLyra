// Fill out your copyright notice in the Description page of Project Settings.

#include "MyLyraPlayerController.h"
#include "MyLyra/Camera/MyLyraPlayerCameraManager.h"

AMyLyraPlayerController::AMyLyraPlayerController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PlayerCameraManagerClass = AMyLyraPlayerCameraManager::StaticClass();
}
