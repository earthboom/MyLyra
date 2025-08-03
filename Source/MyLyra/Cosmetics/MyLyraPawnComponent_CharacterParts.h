// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyLyraCharacterPartTypes.h"
#include "MyLyraCosmeticAnimationTypes.h"
#include "Components/PawnComponent.h"
#include "MyLyraPawnComponent_CharacterParts.generated.h"

class UMyLyraPawnComponent_CharacterParts;

/** Instance화 된 Character Part의 단위 */
USTRUCT()
struct FMyLyraAppliedCharacterPartEntry
{
	GENERATED_BODY()

	/** Character Part 정의 (meta data) */
	UPROPERTY()
	FMyLyraCharacterPart Part;

	/** MyLyraCharacterPartList에서 할당 받은 Part 핸들 값
	 * FMyLyraControllerCharacterPartEntry의 Handle값과 같아야 함 -> 같으면 같은 Part
	 */
	UPROPERTY()
	int32 PartHandle = INDEX_NONE;

	/** Instance화 된 Character Part용 Actor */
	UPROPERTY()
	TObjectPtr<UChildActorComponent> SpawnedComponent = nullptr;
};

/** MyLyraPawnComponent_CharacterParts에서 실질적 Character Parts를 관리하는 클래스 */
USTRUCT()
struct FMyLyraCharacterPartList
{
	GENERATED_BODY()

	FMyLyraCharacterPartList()
		: OwnerComponent(nullptr)
	{}

	FMyLyraCharacterPartList(UMyLyraPawnComponent_CharacterParts* InOwnerComponent)
		: OwnerComponent(InOwnerComponent)
	{}

	bool SpawnActorForEntry(FMyLyraAppliedCharacterPartEntry& Entry);
	void DestoryActorForEntry(FMyLyraAppliedCharacterPartEntry& Entry);
	
	FMyLyraCharacterPartHandle AddEntry(FMyLyraCharacterPart NewPart);
	void RemoveEntry(FMyLyraCharacterPartHandle Handle);

	FGameplayTagContainer CollectCombinedTags() const;

	/** 현재 Instance화된 Character Part */
	UPROPERTY()
	TArray<FMyLyraAppliedCharacterPartEntry> Entries;

	/** 해당 MyLyraCharacterPartList의 Owner인 PawnComponent */
	UPROPERTY()
	TObjectPtr<UMyLyraPawnComponent_CharacterParts> OwnerComponent;

	/** 앞서 보았던 PartHandle의 값을 할당 및 관리하는 변수 */
	int32 PartHandleCounter = 0;
};

/**
 * 
 */
UCLASS()
class MYLYRA_API UMyLyraPawnComponent_CharacterParts : public UPawnComponent
{
	GENERATED_BODY()

public:
	UMyLyraPawnComponent_CharacterParts(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	USkeletalMeshComponent* GetParentMeshComponent() const;
	USceneComponent* GetSceneComponentToAttachTo() const;

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = Cosmetics)
	FGameplayTagContainer GetCombinedTags(FGameplayTag RequiredPrefix) const;

	void BroadcastChanged();
	
	FMyLyraCharacterPartHandle AddCharacterPart(const FMyLyraCharacterPart& NewPart);
	void RemoveCharacterPart(FMyLyraCharacterPartHandle Handle);
	
	/** Instance화 된 Character Parts */
	UPROPERTY()
	FMyLyraCharacterPartList CharacterPartList;

	/** Animation 적용을 위한 Mesh와 연결고리 */
	UPROPERTY(EditAnywhere, Category = Cosmetics)
	FMyLyraAnimBodyStyleSelectionSet BodyMeshes;
};
