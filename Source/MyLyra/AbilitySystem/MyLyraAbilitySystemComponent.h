// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "MyLyraAbilitySystemComponent.generated.h"

/**
 * 
 */
UCLASS()
class MYLYRA_API UMyLyraAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	UMyLyraAbilitySystemComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor) override;
	
	void AbilityInputTagPressed(const FGameplayTag& InputTag);
	void AbilityInputTagReleased(const FGameplayTag& InputTag);
	void ProcessAbilityInput(float DeltaTime, bool bGamePaused);

	/** Ability Input 처리할 Pending Queue */
	TArray<FGameplayAbilitySpecHandle> InputPressedSpecHandles;		// 눌렀을 때
	TArray<FGameplayAbilitySpecHandle> InputReleasedSpecHandles;	// 땠을 때
	TArray<FGameplayAbilitySpecHandle> InputHeldSpecHandles;		// 누르는 동안
};
