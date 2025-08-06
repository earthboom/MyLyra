// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyLyraWeaponInstance.h"
#include "MyLyraRangedWeaponInstance.generated.h"

/**
 * 
 */
UCLASS()
class MYLYRA_API UMyLyraRangedWeaponInstance : public UMyLyraWeaponInstance
{
	GENERATED_BODY()

public:
	/** 유효 사거리 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WeaponConfig", meta=(ForceUnits = cm))
	float MaxDamageRange = 25000.0f;

	/** 총탄 범위 (Sphere Trace Sweep) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WeaponConfig", meta=(ForceUnits = cm))
	float BulletTraceWeaponRadius = 0.0f;
};
