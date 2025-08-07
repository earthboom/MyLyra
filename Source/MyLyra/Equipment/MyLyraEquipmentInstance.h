// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "MyLyraEquipmentInstance.generated.h"

struct FMyLyraEquipmentActorToSpawn;
/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class MYLYRA_API UMyLyraEquipmentInstance : public UObject
{
	GENERATED_BODY()

public:
	UMyLyraEquipmentInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintImplementableEvent, Category = Equipment, meta = (DisplayName = "OnEquipped"))
	void K2_OnEquipped();

	UFUNCTION(BlueprintImplementableEvent, Category = Equipment, meta = (DisplayName = "OnUnequipped"))
	void K2_OnUnequipped();

	UFUNCTION(BlueprintPure, Category = Equipment)
	TArray<AActor*> GetSpawnedActor() const { return SpawnedActors;}
	
	UFUNCTION(BlueprintPure, Category = Equipment)
	APawn* GetPawn() const;

	void SpawnEquipmentActors(const TArray<FMyLyraEquipmentActorToSpawn>& ActorsToSpawn);
	void DestroyEquipmentActors();

	/**
	 * DetermineOutPutType은 C++ 정의에는 APawn* 반환하지만, BP에선 PawnType에 따라 OutputType이 결정되도록 Redirect
	 */
	UFUNCTION(BlueprintPure, Category = Equipment, meta = (DeterminesOutputType = PawnType))
	APawn* GetTypedPawn(TSubclassOf<APawn> PawnType) const;	
	
	/** Interface */
	virtual void OnEquipped();
	virtual void OnUnequipped();

	/** 어떤 InventoryItemInstance에 의해 활성화되었는지 (추후, QuickBarComponent와 연관 ) */
	UPROPERTY()
	TObjectPtr<UObject> Instigator;

	/** MyLyraEquipmentDefinition에 맞게 Spawn된 Actor Instance들 */
	UPROPERTY()
	TArray<TObjectPtr<AActor>> SpawnedActors;
};
