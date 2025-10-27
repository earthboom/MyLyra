// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "MyLyraUserFacingExperience.generated.h"

class UCommonSession_HostSessionRequest;
/**
 * 1. '사용자에게 보여지는(User-Facing)' 게임 경험을 정의하는 DataAsset
 * - 게임 로비나 모드 선택 화면에서 유저가 클릭하게 될 '카드' 하나하나에 대한 정보를 담고 있는 설계도라고 생각
 *
 * 2. '데이터 기반(Dara-Driven) 아키텍처'와 '모듈성'을 명확히 보여주는 구조
 * Decoupling
 * - UI(사용자가 보는 것)와 실제 게임 로직(ExperienceDefinition) 및 레벨(Map)을 완전히 분리
 * - UI는 이 UMyLyraUserFacingExperence 에셋의 목록만 보여주면 될 뿐, 각 모드가 어떤 맵에서 어떤 규칙으로 동작하는지 알 필요 없음
 * 디자이너 친화적
 * - 코드 수정 없이, 기획자나 디자이너가 언리얼 에디터에서 이 데이터 에셋을 생성하고 조합하는 것만으로 새로운 게임 모드 선택지를 얼마든지 만들 수 있음
 * 모듈형 게임 기능과 연동
 * - ExperienceID가 가리키는 UMyLyraExperienceDefinition은 해당 게임 경험에 필요한 특정 게임 기능 플러그인(Game Feature Plugin)을 활성화하는 역할
 * - 이 클래스는 모듈형 아키텍처의 시작점이 되는 중요한 연결고리
 * - Ex) '무기 생성' 기능이 필요한 모드라면, 관련 Experience Definition이 'ShooterCore' 같은 게임 기능 플러그인을 로드하도록 설정할 수 있음
 *
 * 3. 다른 시스템과의 상호작용
 * UI
 * - 게임 모드 선택 UI는 이 에셋을 읽어와 목록을 구성
 * - 사용자가 특정 모드를 선택하고 '게임 시작'을 누르면, 이 에셋의 CreateHotingRequest 함수가 호출되어 세션 생성 프로세스가 시작됨
 * Game Modes (Experience)
 * - 세션이 생성되면, 시스템은 이 에셋에 정의된 MapID로 맵을 로드하고, ExperienceID에 해당하는 UMyLyraExperienceDefinition을 활성화하여 게임의 규칙, 플레이어 설정, GAS 초기화 등을 진행
 *
 * 결론
 * UI와 실제 게임플레이 로직 사이를 잇는 데이터 기반의 '메뉴판' 역할을 수행
 */
UCLASS(BlueprintType)
class MYLYRA_API UMyLyraUserFacingExperience : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	/** Map 로딩 및 Experience 전환을 위해, MapID와 ExperienceID를 활용하며, HostSessionRequest 생성 */
	UFUNCTION(BlueprintCallable, BlueprintPure = false)
	UCommonSession_HostSessionRequest* CreateHostingRequest() const;

	/** the specific map to load
	 *	이 '경험'을 플레이할 특정 맵 (레벨)
	 *	FPrimaryAssetId 를 통해 다른 UPrimaryDataAsset을 가리키고 있음
	 *	- FPrimaryAssetId는 Database의 Primary Key와 같은 느낌으로 생각하면 쉬움 (더 쉽게는 DataAsset의 ID라고 생각해도 무방함)
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Experience, meta = (AllowedTypes = "Map"))
	FPrimaryAssetId MapID;

	/** the gameplay experience to load */
	/**
	 * 해당 맵에서 실행될 게임플레이 규칙의 집합 (UMyLyraExperienceDefinition)
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Experience, meta = (AllowedTypes = "MyLyraExperienceDefinition"))
	FPrimaryAssetId ExperienceID;
};

/**
 * 예시
 * '팀 데스매치'와 '깃발 뺏기'라는 두 가지 게임 모드가 있고, 'Arctic'라는 맵이 있다고 가정
 * 디자이너는 다음과 같은 두 개의 UMyLyraUserFacingExperience 에셋을 만들 수 있다.
 *
 * - Arctic_TDM : MapID = 'Arctic' 맵 / ExperienceID = '팀 데스매치' 규칙을 가리킴
 * - Arctic_CTF : MapID = 'Arctic' 맵 / ExperienceID = '깃발 뺏기' 규칙을 가리킴
 */
