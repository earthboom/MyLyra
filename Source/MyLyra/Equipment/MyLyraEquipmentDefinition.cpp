// Fill out your copyright notice in the Description page of Project Settings.

#include "MyLyraEquipmentDefinition.h"
#include "MyLyraEquipmentInstance.h"

UMyLyraEquipmentDefinition::UMyLyraEquipmentDefinition(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// 기본값
	InstanceType = UMyLyraEquipmentInstance::StaticClass();
}
