// Fill out your copyright notice in the Description page of Project Settings.

#include "MyLyraWeaponUserInterface.h"
#include "MyLyra/Equipment/MyLyraEquipmentManagerComponent.h"
#include "MyLYra/Weapons/MyLyraWeaponInstance.h"

UMyLyraWeaponUserInterface::UMyLyraWeaponUserInterface(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UMyLyraWeaponUserInterface::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// Lyra에선 Tick 마다 검사를 하고 있음
	// 이벤트에 의해 검사하는 것이 좋다고 보는데.... 왜 Lyra는 이런 구성을 했을까?
	if (APawn* Pawn = GetOwningPlayerPawn())
	{
		// EquipmentManagerComponent를 활용해, WeaponInstnace 가져옮
		if (UMyLyraEquipmentManagerComponent* EquipmentManager = Pawn->FindComponentByClass<UMyLyraEquipmentManagerComponent>())
		{
			if (UMyLyraWeaponInstance* NewInstance = EquipmentManager->GetFirstInstanceOfType<UMyLyraWeaponInstance>())
			{
				if (NewInstance != CurrentInstance && NewInstance->GetInstigator() != nullptr)
				{
					// 업데이트
					UMyLyraWeaponInstance* OldWeapon = CurrentInstance;
					CurrentInstance = NewInstance;
					OnWeaponChanged(OldWeapon, CurrentInstance);
				}
			}
		}
	}
}
