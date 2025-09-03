// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "MyLyraWeaponUserInterface.generated.h"

class UMyLyraWeaponInstance;

/**
 * 
 */
UCLASS()
class MYLYRA_API UMyLyraWeaponUserInterface : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	UMyLyraWeaponUserInterface(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** Weapon 변경에 따른 BP Event */
	UFUNCTION(BlueprintImplementableEvent)
	void OnWeaponChanged(UMyLyraWeaponInstance* OldWeapon, UMyLyraWeaponInstance* NewWeapon);

	/** UUserWidget's Interface */
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	/** 현재 장착된 WeaponInstance를 추적 (NativeTick을 활용해 주기적 업데이트) */
	UPROPERTY(Transient)
	TObjectPtr<UMyLyraWeaponInstance> CurrentInstance;
};
