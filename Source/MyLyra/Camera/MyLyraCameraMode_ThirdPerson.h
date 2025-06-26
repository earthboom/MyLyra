// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyLyraCameraMode.h"
#include "MyLyraCameraMode_ThirdPerson.generated.h"

class UCurveVector;

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class MYLYRA_API UMyLyraCameraMode_ThirdPerson : public UMyLyraCameraMode
{
	GENERATED_BODY()

public:
	UMyLyraCameraMode_ThirdPerson(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void UpdateView(float DeltaTime) override;

	UPROPERTY(EditDefaultsOnly, Category = "Third Person")
	TObjectPtr<const UCurveVector> TargetOffsetCurve;
};
