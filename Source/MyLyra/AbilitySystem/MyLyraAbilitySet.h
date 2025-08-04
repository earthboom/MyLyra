// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "MyLyraAbilitySet.generated.h"

struct FGameplayAbilitySpecHandle;
class UMyLyraAbilitySystemComponent;
class UMyLyraGameplayAbility;

/**
 * GameplayAbility의 Wrapper 클래스
 * - 추가적인 커스터마이징 가능
 */
USTRUCT(BlueprintType)
struct FMyLyraAbilitySet_GameplayAbility
{
	GENERATED_BODY()

public:
	/** 허용된 GameplayAbility */
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UMyLyraGameplayAbility> Ability = nullptr;

	/** Input 처리를 위한 GameplayTag */
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag InputTag;

	/** Ability의 허용 조건 (Level) */
	UPROPERTY(EditDefaultsOnly)
	int32 AbilityLevel = 1;
};

USTRUCT(BlueprintType)
struct FMyLyraAbilitySet_GrantedHandles
{
	GENERATED_BODY()

	void AddAbilitySpecHande(const FGameplayAbilitySpecHandle& Handle);
	// void TakeFromAbilitySystem(UMyLyraAbilitySystemComponent* MyLyraASC);

protected:
	/** 허용된 GameplayAbilitySpecHandle (int32) */
	UPROPERTY()
	TArray<FGameplayAbilitySpecHandle> AbilitySpecHandles;
};

/**
 * 
 */
UCLASS()
class MYLYRA_API UMyLyraAbilitySet : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UMyLyraAbilitySet(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** ASC에 허용 가능한 Ability를 추가 */
	void GiveToAbilitySystem(UMyLyraAbilitySystemComponent* MyLyraASC, FMyLyraAbilitySet_GrantedHandles* OutGrantedHandles, UObject* SourceObject = nullptr);
	
	/** 허용된 GameplayAbilityies */
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Abilities")
	TArray<FMyLyraAbilitySet_GameplayAbility> GrantedGameplayAbilities;
};
