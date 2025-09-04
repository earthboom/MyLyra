// Fill out your copyright notice in the Description page of Project Settings.

#include "MyLyraExperienceActionSet.h"
#include "GameFeatureAction.h"

UMyLyraExperienceActionSet::UMyLyraExperienceActionSet()
{
}

#if WITH_EDITORONLY_DATA
void UMyLyraExperienceActionSet::UpdateAssetBundleData()
{
	Super::UpdateAssetBundleData();

	for (UGameFeatureAction* Action : Actions)
	{
		if (Action)
		{
			Action->AddAdditionalAssetBundleData(AssetBundleData);
		}
	}
}
#endif
