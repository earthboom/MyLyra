// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UIExtensionSystem.h"
#include "Components/DynamicEntryBoxBase.h"
#include "UIExtensionPointWidget.generated.h"

/**
 * UIExntesionPointWIdget은 UIExtension 하나 단위로 생각
 * - UIExtension에 결합된 Cntext 별로 관리하는 객체가 UIExtensionPoint(Handle)
 * - 예 ) UIExntesion Slot에 PlayerState / PlayerController / LocalPlayer와 같다.
 * 다양한 Context로부터 Widget이 결합될 수 있다.
 * 이를 분리하여 관리하기 위해 만들어진 객체가 UIExntesionPoint
 */
UCLASS()
class UIEXTENSION_API UUIExtensionPointWidget : public UDynamicEntryBoxBase
{
	GENERATED_BODY()

public:
	UUIExtensionPointWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI Extension")
	FGameplayTag ExtensionPaintTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI Extension")
	EUIExtensionPointMatch ExtensionPointTagMatch = EUIExtensionPointMatch::ExactMatch;

	/** UIExtensionPointWidget에 결합이 허용된 UWidget Classes */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI Extension")
	TArray<TObjectPtr<UClass>> DataClasses;

	/** UIExtension - Widget 관계 매핑 */
	UPROPERTY(Transient)
	TMap<FUIExtensionHandle, TObjectPtr<UUserWidget>> ExtensionMapping;

	/** UIExtensionPoint 객체 관리 */
	TArray<FUIExtensionPointHandle> ExtensionPointHandles;
};
