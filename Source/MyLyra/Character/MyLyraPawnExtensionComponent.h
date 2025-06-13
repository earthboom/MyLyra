// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PawnComponent.h"
#include "Components/GameFrameworkInitStateInterface.h"
#include "MyLyraPawnExtensionComponent.generated.h"

/**
 * 초기화 전반을 조정하는 Component
 * Owner의 Component들의 초기화를 중재하는 역할?
 *
 * IGameFrameworkInitStateInterface : InitState 를 편리하게 관리하기 위한 Interface
 */
UCLASS()
class MYLYRA_API UMyLyraPawnExtensionComponent : public UPawnComponent, public IGameFrameworkInitStateInterface
{
	GENERATED_BODY()

public:
	UMyLyraPawnExtensionComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// FeatureName 정의
	static const FName NAME_ActorFeatureName;

	/**
	 * UPawnComponent Interface
	 */
	// 생성 초반에 호출되며, 이 단계에서 Actor에 Component를 부착하는 단계
	virtual void OnRegister() override;

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/**
	 * IGameFrameworkInitStateInterface 
	 */
	virtual FName GetFeatureName() const final;
	virtual void OnActorInitStateChanged(const FActorInitStateChangedParams& Params) final;
	virtual bool CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const final;
	virtual void CheckDefaultInitialization() final;
};
