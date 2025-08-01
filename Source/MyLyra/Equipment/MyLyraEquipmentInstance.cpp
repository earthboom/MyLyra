// Fill out your copyright notice in the Description page of Project Settings.

#include "MyLyraEquipmentInstance.h"
#include "MyLyraEquipmentDefinition.h"
#include "GameFramework/Character.h"

UMyLyraEquipmentInstance::UMyLyraEquipmentInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

APawn* UMyLyraEquipmentInstance::GetPawn() const
{
	return Cast<APawn>(GetOuter());
}

void UMyLyraEquipmentInstance::SpawnEquipmentActors(const TArray<FMyLyraEquipmentActorToSpawn>& ActorsToSpawn)
{
	APawn* OwningPawn = GetPawn();
	if (IsValid(OwningPawn))
	{
		// 현재 Owner인 Pawn 의 RootComponent 를 AttachTarget 대상으로 함
		USceneComponent* AttachTarget = OwningPawn->GetRootComponent();
		ACharacter* Char = Cast<ACharacter>(OwningPawn);
		if (IsValid(Char))
		{
			// 만일 캐릭터라면, SkeletalMeshComponent가 있으면 GetMesh로 반환해서 붙임
			AttachTarget = Char->GetMesh();
		}

		for (const FMyLyraEquipmentActorToSpawn& SpawnInfo : ActorsToSpawn)
		{
			// SpawnActorDeferred는 FinishSpawning을 호출해야 Spawn이 완성됨( 즉, 작성자에게 코드로서 Ownership이 있다는 의미 )
			AActor* NewACtor = GetWorld()->SpawnActorDeferred<AActor>(SpawnInfo.ActorToSpawn, FTransform::Identity, OwningPawn);
			NewACtor->FinishSpawning(FTransform::Identity, true);

			// Actor의 RelativeTransform을 AttachTransform으로 설정
			NewACtor->SetActorRelativeTransform(SpawnInfo.AttachTransform);

			// AttachTarget에 붙이자 (Actor -> Actor)
			NewACtor->AttachToComponent(AttachTarget, FAttachmentTransformRules::KeepRelativeTransform, SpawnInfo.AttachSocket);

			SpawnedActors.Add(NewACtor);
		}
	}
}

void UMyLyraEquipmentInstance::DestroyEquipmentActors()
{
	for (AActor* Actor : SpawnedActors)
	{
		if (IsValid(Actor))
		{
			Actor->Destroy();
		}
	}
}

void UMyLyraEquipmentInstance::OnEquipped()
{
	K2_OnEquipped();
}

void UMyLyraEquipmentInstance::OnUnequipped()
{
	K2_OnUnequipped();
}
