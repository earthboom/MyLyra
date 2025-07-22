// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyLyraCharacterPartTypes.generated.h"

USTRUCT(BlueprintType)
struct FMyLyraCharacterPartHandle
{
	GENERATED_BODY()

	void Reset()
	{
		PartHandle = INDEX_NONE;
	}

	bool IsValid() const
	{
		return PartHandle != INDEX_NONE;
	}
	
	UPROPERTY()
	int32 PartHandle = INDEX_NONE;
};

USTRUCT(BlueprintType)
struct FMyLyraCharacterPart
{
	GENERATED_BODY()

	/** Part 로 생성된 Actor의 Class 정보를 가지고 있음
	 * -> Actor BP 를 하나의 파츠로 여기면 됨
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> PartClass;

	/** Actor 를 소환하여 붙이는데 특정 Bone에 붙이기 위한 정보
	 * - 따로 정의되지 않으면 Root에 붙임
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite);
	FName SocketName;
};