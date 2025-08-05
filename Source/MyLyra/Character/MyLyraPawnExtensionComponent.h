// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyLyra/Character/MyLyraPawnData.h"
#include "Components/PawnComponent.h"
#include "Components/GameFrameworkInitStateInterface.h"
#include "MyLyraPawnExtensionComponent.generated.h"

class UMyLyraAbilitySystemComponent;
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
	 * member methods
	 */
	static UMyLyraPawnExtensionComponent* FindPawnExtensionComponent(const AActor* Actor) { return (IsValid(Actor) ? Actor->FindComponentByClass<UMyLyraPawnExtensionComponent>() : nullptr); }

	template <class T>
	const T* GetPawnData() const;

	void SetPawnData(const UMyLyraPawnData* InPawnData);
	void SetupPlayerInputComponent();

	UMyLyraAbilitySystemComponent* GetMyLyraAbilitySystemComponent() const { return AbilitySystemComponent; }

	/** AbilitySystemComponent의 AvatarActor 대상 초기화 / 해제 호출 */
	void InitializeAbilitySystem(UMyLyraAbilitySystemComponent* InASC, AActor* InOwnerActor);
	void UnInitializeAbilitySystem();

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
	virtual void CheckDefaultInitialization() final;

	/**
	 * Pawn을 생성한 데이터 캐싱
	 */
	UPROPERTY(EditInstanceOnly, Category = "MyLyra|Pawn")
	TObjectPtr<const UMyLyraPawnData> PawnData;

	/** AbilitySystemComponent 캐싱 */
	UPROPERTY()
	TObjectPtr<UMyLyraAbilitySystemComponent> AbilitySystemComponent;
};

template <class T>
const T* UMyLyraPawnExtensionComponent::GetPawnData() const
{
	return Cast<T>(PawnData);
}
