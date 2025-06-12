// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "MyLyraPawnData.generated.h"

/**
 * 어떤 Pawn을 생성하는지, 입력을 어떻게 하는지, 스킬 뭘 사용하는지 와 같은 데이터 , Ability 를 뭘 들고 있는지
 */
UCLASS()
class MYLYRA_API UMyLyraPawnData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UMyLyraPawnData(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// Pawn class
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MyLyra|Pawn")
	TSubclassOf<APawn> PawnClass;
};
