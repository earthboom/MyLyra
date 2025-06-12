// Copyright Epic Games, Inc. All Rights Reserved.

#include "MyLyra.h"
#include "MyLyraLogChannels.h"
#include "Modules/ModuleManager.h"

class FMyLyraGameModule : public FDefaultGameModuleImpl
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

void FMyLyraGameModule::StartupModule()
{
	FDefaultGameModuleImpl::StartupModule();
	UE_LOG(LogMyLyra, Warning, TEXT("StartupModule!"));
}

void FMyLyraGameModule::ShutdownModule()
{
	FDefaultGameModuleImpl::ShutdownModule();
}

IMPLEMENT_PRIMARY_GAME_MODULE(FMyLyraGameModule, MyLyra, "MyLyra");
