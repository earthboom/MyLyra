// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "MyLyraCosmeticAnimationTypes.generated.h"

USTRUCT(BlueprintType)
struct FMyLyraAnimBodyStyleSelectionEntry
{
	GENERATED_BODY()

	/** Anim Layer 를 적용할 대상 SkeletalMesh */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<USkeletalMesh> Mesh = nullptr;

	/** Cosmetic Tag */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Categories = "Cosmetics"))
	FGameplayTagContainer RequiredTags;
};

USTRUCT(BlueprintType)
struct FMyLyraAnimBodyStyleSelectionSet
{
	GENERATED_BODY()

	/** Gameplaytag를 통해, Mesh Rules에 따라 알맞은 BodyStyle을 반환 */
	USkeletalMesh* SelectBestBodyStyle(const FGameplayTagContainer& CosmeticTags) const;

	/** AnimLayer 적용할 SkeletalMesh를 들고 있음
	 * - Animation - Mesh 간 Rules을 MeshRules라고 여겨라
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FMyLyraAnimBodyStyleSelectionEntry> MeshRules;

	/** Default로 적용할 SkeleataMesh */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<USkeletalMesh> DefaultMesh = nullptr;

	/** Physics Asset은 하나로 통일
	 * - 즉 모든 Animation의 Physics 속성 공유
	 */
	UPROPERTY(EditAnywhere)
	TObjectPtr<UPhysicsAsset> ForcedPhysicsAsset = nullptr;
};
