// Fill out your copyright notice in the Description page of Project Settings.

#include "MyLyraPawnComponent_CharacterParts.h"
#include "GameplayTagAssetInterface.h"
#include "GameFramework/Character.h"
#include "PhysicsEngine/PhysicsAsset.h"

bool FMyLyraCharacterPartList::SpawnActorForEntry(FMyLyraAppliedCharacterPartEntry& Entry)
{
	bool bCreatedAnyActor = false;

	// 전달된 AppliedCharacterPartEntry의 Part Class가 제대로 세팅되어 있으면
	if (Entry.Part.PartClass != nullptr)
	{
		// OwnerComponent의 Owner에 속한 World 반환
		UWorld* World = OwnerComponent->GetWorld();

		// MyLyraPawnComponent_CharacterParts에 어느 Component에 붙일 것인지 결정 ( GetSceneComponentToAttachTo )
		USceneComponent* ComponentToAttachTo = OwnerComponent->GetSceneComponentToAttachTo();
		if (IsValid(ComponentToAttachTo))
		{
			// 붙일 Component인 ComponentToAttachTo의 Bone 혹은 SocketName을 통해 어디에 붙일지 Transform 계산
			const FTransform SpawnTransform = ComponentToAttachTo->GetSocketTransform(Entry.Part.SocketName);

			// Actor - Actor 의 결합이라, ChildActorComponent 활용
			UChildActorComponent* PartComponent = NewObject<UChildActorComponent>(OwnerComponent->GetOwner());
			PartComponent->SetupAttachment(ComponentToAttachTo, Entry.Part.SocketName);
			PartComponent->SetChildActorClass(Entry.Part.PartClass);

			// RegisterComponent를 통해 마지막으로 RenderWorld인 FScene에 변경 내용을 전달 ( or 생성 )
			PartComponent->RegisterComponent();

			// ChildActorComponent 에서 생성한 Actor 반환
			AActor* SpawnedActor = PartComponent->GetChildActor();
			if (IsValid(SpawnedActor))
			{
				// 해당 Actor가 Parent인 MyLyraPawnComponent_CharacterParts의 Owner Actor보다 먼저 Tick이 실행되지 않도록 선행조건을 붙임
				USceneComponent* SpawnedRootComponent = SpawnedActor->GetRootComponent();
				if (IsValid(SpawnedRootComponent))
				{
					SpawnedRootComponent->AddTickPrerequisiteComponent(ComponentToAttachTo);
				}
			}

			Entry.SpawnedComponent = PartComponent;
			bCreatedAnyActor = true;
		}
	}

	return bCreatedAnyActor;
}

void FMyLyraCharacterPartList::DestoryActorForEntry(FMyLyraAppliedCharacterPartEntry& Entry)
{
	if (IsValid(Entry.SpawnedComponent))
	{
		Entry.SpawnedComponent->DestroyComponent();
		Entry.SpawnedComponent = nullptr;
	}
}

FMyLyraCharacterPartHandle FMyLyraCharacterPartList::AddEntry(FMyLyraCharacterPart NewPart)
{
	// PawnComponent의 CharacterPartList가 PartHandle를 관리하고, 이를 ControllerComponent_CharacterParts에 전달
	FMyLyraCharacterPartHandle Result;
	Result.PartHandle = PartHandleCounter++;

	// Authority가 있으면, AppliedCharacterPartEntry를 Entries에 추가
	if (ensure(OwnerComponent && OwnerComponent->GetOwner() && OwnerComponent->GetOwner()->HasAuthority()))
	{
		FMyLyraAppliedCharacterPartEntry& NewEntry = Entries.AddDefaulted_GetRef();
		NewEntry.Part = NewPart;
		NewEntry.PartHandle = Result.PartHandle;

		// 실제 Actor를 생성, OwnerComponent의 Owner Actor에 Actor끼리 RootComponent로 Attach
		if (SpawnActorForEntry(NewEntry))
		{
			// BroadcastChanged를 통해, OwnerComponent에서 Owner의 SkeletalMeshComponent를 활용하여, Animation 및 Physics를 Re-Initialize
			OwnerComponent->BroadcastChanged();
		}
	}

	return Result;
}

void FMyLyraCharacterPartList::RemoveEntry(FMyLyraCharacterPartHandle Handle)
{
	for (auto EntryIt = Entries.CreateIterator(); EntryIt; ++EntryIt)
	{
		FMyLyraAppliedCharacterPartEntry& Entry = *EntryIt;
		
		if (Entry.PartHandle == Handle.PartHandle)
		{
			DestoryActorForEntry(Entry);
		}
	}
}

FGameplayTagContainer FMyLyraCharacterPartList::CollectCombinedTags() const
{
	FGameplayTagContainer Result;

	// Entires 순회
	for (const FMyLyraAppliedCharacterPartEntry& Entry : Entries)
	{
		// Part Actor가 생성되어 Spawncomponent에 캐싱되어 있으면
		if (IsValid(Entry.SpawnedComponent))
		{
			/**
			 * 해당 Actor의 IGameplayTagAssetInterface를 통해 GameplayTag를 검색
			 * - 현재 TaggedActor는 IGameplayTagAssetInterface를 상속받지 않으므로 그냥 넘어 감
			 * - 후일 각 Part에 대해 GameplayTag 를 넣고 싶으면 이걸 상속받아 정의해야 함
			 * - 예 : 특정 Lv100 이상 장착 가능한 장비를 만들 때
			 */
			IGameplayTagAssetInterface* TagInterface = Cast<IGameplayTagAssetInterface>(Entry.SpawnedComponent->GetChildActor());
			if (TagInterface)
			{
				TagInterface->GetOwnedGameplayTags(Result);
			}
		}
	}
	
	return Result;
}

UMyLyraPawnComponent_CharacterParts::UMyLyraPawnComponent_CharacterParts(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, CharacterPartList(this)
{
}

USkeletalMeshComponent* UMyLyraPawnComponent_CharacterParts::GetParentMeshComponent() const
{
	AActor* OwnerActor = GetOwner();
	 if (IsValid(OwnerActor))
	 {
	 	ACharacter* OwningCharacter = Cast<ACharacter>(OwnerActor);
	 	if (IsValid(OwningCharacter))
	 	{
	 		USkeletalMeshComponent* MeshComponent = OwningCharacter->GetMesh();
	 		if (IsValid(MeshComponent))
	 		{
	 			return MeshComponent;
	 		}
	 	}
	 }

	return nullptr;
}

USceneComponent* UMyLyraPawnComponent_CharacterParts::GetSceneComponentToAttachTo() const
{
	// Parent에 SkeletalMeshComponent가 있으면 반환
	if (USkeletalMeshComponent* MeshComponent = GetParentMeshComponent())
	{
		return MeshComponent;
	}
	// RootComponent 확인
	else if (AActor* OwnerActor = GetOwner())
	{
		return OwnerActor->GetRootComponent();
	}

	return nullptr;
}

FGameplayTagContainer UMyLyraPawnComponent_CharacterParts::GetCombinedTags(FGameplayTag RequiredPrefix) const
{
	// 현재 장착된 CharacterPartList의 Merge된 Tags 반환
	FGameplayTagContainer Result = CharacterPartList.CollectCombinedTags();
	if (RequiredPrefix.IsValid())
	{
		return Result.Filter(FGameplayTagContainer(RequiredPrefix));
	}
	else
	{
		return Result;
	}
}

void UMyLyraPawnComponent_CharacterParts::BroadcastChanged()
{
	const bool bReinitPose = true;

	// 현재 Owner의 SkeletalMeshComponent
	USkeletalMeshComponent* MeshComponent = GetParentMeshComponent();
	if (IsValid(MeshComponent))
	{
		// BodyMeshes를 통해, GameplayTag를 활용해, 알맞은 SkeletalMesh로 재설정
		const FGameplayTagContainer MergedTags = GetCombinedTags(FGameplayTag());
		USkeletalMesh* DesiredMesh = BodyMeshes.SelectBestBodyStyle(MergedTags);

		// SkeletalMesh를 초기화 및 Animation 초기화
		MeshComponent->SetSkeletalMesh(DesiredMesh, bReinitPose);

		// PhysicAsset 초기화
		UPhysicsAsset* PhysicsAsset = BodyMeshes.ForcedPhysicsAsset;
		if (IsValid(PhysicsAsset))
		{
			MeshComponent->SetPhysicsAsset(PhysicsAsset, bReinitPose);
		}
	}
}

FMyLyraCharacterPartHandle UMyLyraPawnComponent_CharacterParts::AddCharacterPart(const FMyLyraCharacterPart& NewPart)
{
	return CharacterPartList.AddEntry(NewPart);
}

void UMyLyraPawnComponent_CharacterParts::RemoveCharacterPart(FMyLyraCharacterPartHandle Handle)
{
	CharacterPartList.RemoveEntry(Handle);
}
