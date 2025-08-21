// Fill out your copyright notice in the Description page of Project Settings.

#include "MyLyraHUD.h"
#include "Components/GameFrameworkComponentManager.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(MyLyraHUD)

AMyLyraHUD::AMyLyraHUD(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void AMyLyraHUD::PreInitializeComponents()
{
	Super::PreInitializeComponents();

	// MyLyraHUD를 Receiver로 Actor를 추가
	UGameFrameworkComponentManager::AddGameFrameworkComponentReceiver(this);
}

void AMyLyraHUD::BeginPlay()
{
	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(this, UGameFrameworkComponentManager::NAME_GameActorReady);

	Super::BeginPlay();
}

void AMyLyraHUD::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UGameFrameworkComponentManager::RemoveGameFrameworkComponentReceiver(this);

	Super::EndPlay(EndPlayReason);
}
