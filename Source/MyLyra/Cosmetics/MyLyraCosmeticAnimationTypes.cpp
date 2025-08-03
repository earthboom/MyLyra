// Fill out your copyright notice in the Description page of Project Settings.

#include "MyLyraCosmeticAnimationTypes.h"

TSubclassOf<UAnimInstance> FMyLyraAnimLayerSelectionSet::SelectBestLayer(const FGameplayTagContainer& CosmeticTags) const
{
	for (const FMyLyraAnimLayerSelectionEntry& Rule : LayerRules)
	{
		if (IsValid(Rule.Layer) && CosmeticTags.HasAll(Rule.RequiredTags))
		{
			return Rule.Layer;
		}
	}
	return DefaultLayer;
}

USkeletalMesh* FMyLyraAnimBodyStyleSelectionSet::SelectBestBodyStyle(const FGameplayTagContainer& CosmeticTags) const
{
	// MeshRules 순회, CosmeticTags 요구 조건에 맞는 MeshRule을 찾아 SkeletalMesh 반환
	for (const FMyLyraAnimBodyStyleSelectionEntry& Rule : MeshRules)
	{
		if ((Rule.Mesh) && CosmeticTags.HasAll(Rule.RequiredTags))
		{
			return Rule.Mesh;
		}
	}
	return DefaultMesh;
}
