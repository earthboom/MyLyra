// Fill out your copyright notice in the Description page of Project Settings.

#include "MyLyraCharacter.h"
#include "MyLyraPawnExtensionComponent.h"

// Sets default values
AMyLyraCharacter::AMyLyraCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	// Tick 비활성화
	PrimaryActorTick.bStartWithTickEnabled = false;
	PrimaryActorTick.bCanEverTick = false;

	// PawnExtensionComponent 생성
	PawnExtensionComponent = CreateDefaultSubobject<UMyLyraPawnExtensionComponent>(TEXT("PawnExtensionComponent"));
	// 이후 PawnExtensionComponent 가 관리할 Component 들은 BP에서 생성함으로써, PawnExtensionComponent가 무조건적으로 먼저 생성됨을 보장 받는다.(SCS)
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
