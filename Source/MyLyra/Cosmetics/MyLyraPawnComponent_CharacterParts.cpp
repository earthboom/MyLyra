// Fill out your copyright notice in the Description page of Project Settings.


#include "MyLyraPawnComponent_CharacterParts.h"

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
			OwnerComponent->BroadCastChanged();
		}
	}

	return Result;
}

UMyLyraPawnComponent_CharacterParts::UMyLyraPawnComponent_CharacterParts(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

FMyLyraCharacterPartHandle UMyLyraPawnComponent_CharacterParts::AddCharacterPart(const FMyLyraCharacterPart& NewPart)
{
	return CharacterPartList.AddEntry(NewPart);
}
