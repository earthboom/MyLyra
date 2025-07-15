// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "MyLyraExperienceActionSet.generated.h"

class UGameFeatureAction;
/**
 * 
 */
UCLASS()
class MYLYRA_API UMyLyraExperienceActionSet : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UMyLyraExperienceActionSet();

	/**
	 *	member variables
	 */
	UPROPERTY(EditAnywhere, Category= "Actions to Perform")
	TArray<TObjectPtr<UGameFeatureAction>> Actions;

	
};
