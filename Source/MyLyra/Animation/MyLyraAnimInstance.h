// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "Animation/AnimInstance.h"
#include "MyLyraAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class MYLYRA_API UMyLyraAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeUpdateAnimation(float DeltaTime) override;

	void InitializeWithAbilitySystem(UAbilitySystemComponent* ASC);

	/** 해당 속성값은 Lyra의 AnimBP에서 사용되는 값이므로 정의 */
	UPROPERTY(BlueprintReadOnly, Category = "Character State Data")
	float GroundDistance = -1.0f;

	/** GameplayTag와 AnimInstance의 속성값을 매핑 */
	UPROPERTY(EditDefaultsOnly, Category = "GameplayTags")
	FGameplayTagBlueprintPropertyMap GameplayTagPropertyMap;
};
