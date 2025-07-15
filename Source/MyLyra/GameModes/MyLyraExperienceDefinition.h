// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "MyLyraExperienceDefinition.generated.h"

class UMyLyraExperienceActionSet;
class UGameFeatureAction;
class UMyLyraPawnData;
/**
 * 
 */
UCLASS()
class UMyLyraExperienceDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UMyLyraExperienceDefinition(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** member variables */
	UPROPERTY(EditDefaultsOnly, Category = Gameplay)
	TObjectPtr<UMyLyraPawnData> DefaultPawnData;

	/**
	* 해당 Property는 단순 마킹 및 기억용으로 남겨둔다
	* Game Mode에 따른 GameFeature Plugin 을 로딩하는데 이메 대한 연결고리고 생각하면 됨
	* 후일 ShooterCore 관련 Plugin 작업 진행할 경우, 진행될 예정
	*/
	UPROPERTY(EditDefaultsOnly, Category = Gameplay)
	TArray<FString> GameFeatureToEnable;

	/** ExperienceActionSet은 UGameFeatureAction의 Set이며, Gameplay 용도에 맞게 분류의 목적으로 사용 */
	UPROPERTY(EditDefaultsOnly, Category = Gameplay)
	TArray<TObjectPtr<UMyLyraExperienceActionSet>> ActionSets;

	/** 일반적인 GameFeatureAction으로서 추가 */
	UPROPERTY(EditDefaultsOnly, Category = Action)
	TArray<TObjectPtr<UGameFeatureAction>> Actions;
};
