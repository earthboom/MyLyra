// Fill out your copyright notice in the Description page of Project Settings.


#include "MyLyraCharacter.h"

// Sets default values
AMyLyraCharacter::AMyLyraCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMyLyraCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMyLyraCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AMyLyraCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

