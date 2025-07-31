// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PawnComponent.h"
#include "Components/GameFrameworkInitStateInterface.h"
#include "MyLyra/Input/MyLyraMappableConfigPair.h"
#include "MyLyraHeroComponent.generated.h"

struct FInputActionValue;
class UMyLyraCameraMode;

/*
 * 카메라, 입력 등 플레이어가 제어하는 시스템의 초기화를 처리하는 Component
 */
UCLASS(Blueprintable, Meta = (BlueprintSpawnableComponent))
class MYLYRA_API UMyLyraHeroComponent : public UPawnComponent, public IGameFrameworkInitStateInterface
{
	GENERATED_BODY()

public:
	UMyLyraHeroComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** FeatureName 정의*/
	static const FName NAME_ActorFeatureName;

	/** Extension Event 이름 정의 */
	static const FName NAME_BindInputsNow;

	/**
	 * UPawnComponent Interface
	 */
	virtual void OnRegister() override; // 생성 초반에 호출되며, 이 단계에서 Actor에 Component를 부착하는 단계
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/**
	 * IGameFrameworkInitStateInterface 
	 */
	virtual FName GetFeatureName() const final;
	virtual void OnActorInitStateChanged(const FActorInitStateChangedParams& Params) final;
	virtual bool CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const final;
	virtual void HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) final;
	virtual void CheckDefaultInitialization() final;

	TSubclassOf<UMyLyraCameraMode> DetermineCameraMode() const;
	void InitializePlayerInput(UInputComponent* PlayerInputComponent);
	void Input_Move(const FInputActionValue& InputActionValue);
	void Input_LookMove(const FInputActionValue& InputActionValue);

	UPROPERTY(EditAnywhere)
	TArray<FMyLyraMappableConfigPair> DefaultInputConfigs;
};
