// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "MyLyraExperienceDefinition.generated.h"

class UMyLyraExperienceActionSet;
class UGameFeatureAction;
class UMyLyraPawnData;

/**
 * 특정 게임플레이 경험(Game Experence)을 구성하는 모든 핵심 요소들을 정의하는 데이터 에셋
 * - 게임 모드, 캐릭터, 능력,  UI 등 해당 경험에 필요한 모든 것을 한 곳에 모아놓은 '마스터 설정 파일'
 *
 * 1. 핵심 목적 : "하나의 게임 경"을 완벽하게 정의
 * - 모듈형 게임 기능(Modular Game Featurs)
 * - 이 에셋은 어떤 게임 기능 플러그인(Game Feature Plugin)이 활성화되어야 하는지, 어떤 Action들이 실행되어야 하는지, 어떤 기본 능력이 부여되어야 하는지 등을 명시
 * 
 * 2. 주요 기능
 * 1) DefaultPawnData
 * - 이 경험에서 플레이어가 사용할 기본 폰(Pawn)의 데이터를 지정
 * - UMyLyraPawnData는 해당 폰의 메시, 애니메이션, 능력 등 다양한 정보를 담고 있을 것
 * 2) GameFeaturesToEnable
 * - 이 경험이 활성화될 때 로드하고 활성화해야 할 게임 기능 플러그인(Game Feature Plugin)들의 이름 목록
 * 3) AbilitySets
 * - 이 경험이 시작될 때 실행될 액션들의 '세트'를 정의
 * 4) Actions
 * - 이 경험이 활성화될 때 직접 실행될 일련의 개별 Action들을 정의
 * - UGameFeatureActions는 UI 위젯 추가(GameFeatureAction_AddWidgets), 입력 설정 추가(GameFeatureAction_AddInputConfig) 등 다양한 동적 작업을 수행할 수 있는 기본 클래스
 * 
 * 3. 사용 시점
 * 1) 게임 시작 및 모드 선택 시
 * - 사용자가 UMyLyraUserFacingExperience를 통해 특정 게임 모드를 선택하면, 시스템은 해당 UMyLyraUserFacingExperience에 연결된 UMyLyraExperienceDefinition을 로드
 * - 이 UMyLyraExperienceDefinition이 로드되면, UMyLyraExperienceManagerComponent와 같은 시스템이, 이 정의에 따라 게임 환경을 설정
 * 2) 게임플레이 환경 설정 시
 * - GameFeaturesToEnable에 나열된 게임 기능 플러그인들이 활성화
 * - ActionSets와 Actions에 정의된 UGameFeatureAction들이 실행되어 필요한 게임 기능들을 활성화하고, UI 위젯을 추가하며, 입력 설정을 적용하는 등의 작업을 수행
 * - 플레이어가 게임에 참여하거나 리스폰될 때, DefaultPawnData를 참조하여 적절한 폰을 생성하고 초기화
 *
 * 결론
 * - 하나의 게임 경험을 완벽하게 정의하고 구성하는 중앙 집중식 데이터 애셋
 * - 이는 모듈형 게임 기능, GAS(간접적으로 ActionSets나 Actions 내의 UGameFeatureAction을 통해 어빌리티 부여 로직이 포함될 수 있음), 그리고 데이터 기반 UI를 유기적으로 연결하여, 유연하고 확장 가능한 게임 아키텍처를 구축하는 데 핵심적인 역할
 * - 새로운 게임 모드를 추가하거나 기존 모드를 수정할 때, 이 애셋을 통해 대부분의 설정을 처리하게 된다.
 *
 * 2025.10.31
 * - Experience가 로딩 완료되었을 경우, 이벤트를 받아 Experience의 DefaultPawnData를 PlayerState에 설정할 필요가 있다.
 *		- 앞서 MyLyraExperienceManagerComponent의 OnExperienceLoaded에 등록할 필요가 있다
 */
UCLASS()
class UMyLyraExperienceDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UMyLyraExperienceDefinition(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

#if WITH_EDITORONLY_DATA
	virtual void UpdateAssetBundleData() override;
#endif
	
	/** member variables
	 * - 단순히 Pawn을 Spawn하기 위한 정보를 담고 있다고 이해하자
	 */
	UPROPERTY(EditDefaultsOnly, Category = Gameplay)
	TObjectPtr<UMyLyraPawnData> DefaultPawnData;

	/**
	* 해당 Property는 단순 마킹 및 기억용으로 남겨둔다
	* Game Mode에 따른 GameFeature Plugin 을 로딩하는데 이메 대한 연결고리고 생각하면 됨
	* 후일 ShooterCore 관련 Plugin 작업 진행할 경우, 진행될 예정
	*/
	UPROPERTY(EditDefaultsOnly, Category = Gameplay)
	TArray<FString> GameFeaturesToEnable;

	/** ExperienceActionSet은 UGameFeatureAction의 Set이며, Gameplay 용도에 맞게 분류의 목적으로 사용 */
	UPROPERTY(EditDefaultsOnly, Category = Gameplay)
	TArray<TObjectPtr<UMyLyraExperienceActionSet>> ActionSets;

	/** 일반적인 GameFeatureAction으로서 추가 */
	UPROPERTY(EditDefaultsOnly, Instanced, Category = "Actions")
	TArray<TObjectPtr<UGameFeatureAction>> Actions;
};
