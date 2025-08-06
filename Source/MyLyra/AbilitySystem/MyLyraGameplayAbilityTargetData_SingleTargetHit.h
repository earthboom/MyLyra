#pragma once

#include "Abilities/GameplayAbilityTargetTypes.h"
#include "MyLyraGameplayAbilityTargetData_SingleTargetHit.generated.h"

/**
 * 
 */

USTRUCT()
struct FMyLyraGameplayAbilityTargetData_SingleTargetHit : public FGameplayAbilityTargetData_SingleTargetHit
{
	GENERATED_BODY()

public:
	FMyLyraGameplayAbilityTargetData_SingleTargetHit()
		: CartridgeID(-1)
	{}

	virtual UScriptStruct* GetScriptStruct() const override
	{
		return FMyLyraGameplayAbilityTargetData_SingleTargetHit::StaticStruct();
	}

	/** 탄약 ID */
	UPROPERTY()
	int32 CartridgeID;
};
