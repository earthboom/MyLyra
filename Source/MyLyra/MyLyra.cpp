// Copyright Epic Games, Inc. All Rights Reserved.

#include "MyLyra.h"
#include "MyLyraLogChannels.h"
#include "Modules/ModuleManager.h"

/**
 * FMyLyraGameModule
 * - 이 모듈이 로드되거나 언로드될 때 호출될 코드를 포함하는 모듈 구현 클래스
 * - IModuleInterface를 상속 받음
 * - StartupModule, ShutdownModule 함수를 통해 모듈 초기화 및 정리 로직 수행 가능
 */
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

/**
 * @MyLyra
 * - 언리얼 엔진이 인식하는 모듈의 내부 이름
 * - MyLyra.Build.cs 파일에서 정의된 모듈 이름과 일치해야 한다.
 * @"MyLyra"
 * - UBT(Unreal Build Tool)와 에디터에서 사용되는 게임의 이름
 *
 * ** 매크로의 의미 **
 * 중심축(Hub) 역할
 * - MyLyra 프로젝트는 ShooterCore와 같은 여러 게임 기능 플러그인(Game Feature Plugins)으로 구성됨
 * - IMPLEMENT_PRIMARY_GAME_MODULE로 지정된 MyLyra 모듈은 이 모든 플러그인들이 로드되고 활성화될 수 있는 기반, 중심축 역할을 수행
 * 엔진과의 첫 연결고리
 * - 게임이 시작할 때 언리얼 엔진은 이 "Primary Game Module"을 가장 먼저 찾아 로드하고, 여기서부터 게임의 나머지 부분을 초기화하기 시작
 *
 * 결론
 * "이것이 MyLyra 게임의 시작점이며, 다른 모든 모듈과 플러그인은 이 모듈을 중심으로 동작한다."고 언리얼 엔진에 선언하는 것이다.
 */
IMPLEMENT_PRIMARY_GAME_MODULE(FMyLyraGameModule, MyLyra, "MyLyra");
