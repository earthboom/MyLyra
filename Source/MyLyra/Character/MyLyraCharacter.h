// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "ModularCharacter.h"
#include "MyLyraCharacter.generated.h"

class UMyLyraHealthComponent;
class UMyLyraCameraComponent;
class UMyLyraPawnExtensionComponent;

UCLASS()
class MYLYRA_API AMyLyraCharacter : public AModularCharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMyLyraCharacter();

	void OnAbilitySystemInitialized();
	void OnAbilitySystemUninitialized();

	/** IAbilitySystemInterface*/
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MyLyra|Character")
	TObjectPtr<UMyLyraPawnExtensionComponent> PawnExtensionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MyLyra|Character")
	TObjectPtr<UMyLyraCameraComponent> CameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MyLyra|Character")
	TObjectPtr<UMyLyraHealthComponent> HealthComponent;
};
