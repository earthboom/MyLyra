// Fill out your copyright notice in the Description page of Project Settings.


#include "MyLyraExperienceDefinition.h"
#include "GameFeatureAction.h"

UMyLyraExperienceDefinition::UMyLyraExperienceDefinition(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

#if WITH_EDITORONLY_DATA
void UMyLyraExperienceDefinition::UpdateAssetBundleData()
{
	Super::UpdateAssetBundleData();

	for (UGameFeatureAction* Action : Actions)
	{
		if (IsValid(Action))
		{			
			// AddAdditionalAssetBundleData()는 UGameFeatureAction의 메서드
			// - 임의적 호출을 통해 AssetBundleData에 추가
			Action->AddAdditionalAssetBundleData(AssetBundleData);
		}
	}
}
#endif