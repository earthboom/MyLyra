// Fill out your copyright notice in the Description page of Project Settings.

#include "UIExtensionSystem.h"
#include "Blueprint/UserWidget.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(UIExtensionSystem)

void FUIExtensionHandle::Unregister()
{
	if (UUIExtensionSubsystem* ExtensionSourcePtr = ExtensionSource.Get())
	{
		ExtensionSourcePtr->UnregisterExtension(*this);
	}
}

void UUIExtensionSubsystem::UnregisterExtension(const FUIExtensionHandle& ExtensionHandle)
{
	if (ExtensionHandle.IsValid())
	{
		// 반드시 해당 ExtensionHandle이 UUIExtensionSubsystem과 같은지 확인해야 함
		checkf(ExtensionHandle.ExtensionSource == this, TEXT("Trying to unregister an extension that's not from this extension subsystem"));

		TSharedPtr<FUIExtension> Extension = ExtensionHandle.DataPtr;

		// Extension의 PointTag를 통해 ExtensionMap에서 해당 Slot에 있는지 찾아서 제거
		FExtensionList* ListPtr = ExtensionMap.Find(Extension->ExtensionPointTag);
		if (ListPtr)
		{
			ListPtr->RemoveSwap(Extension);
			if (ListPtr->Num() == 0)
			{
				// Num() == 0이면 Map에서도 제거 진행
				ExtensionMap.Remove(Extension->ExtensionPointTag);
			}
		}
	}
}

FUIExtensionHandle UUIExtensionSubsystem::RegisterExtensionAsWidgetForContext(const FGameplayTag& ExtensionPointTag, UObject* ContextObject, TSubclassOf<UUserWidget> WidgetClass, int32 Priority)
{
	return RegisterExtensionAsData(ExtensionPointTag, ContextObject, WidgetClass, Priority);
}

FUIExtensionHandle UUIExtensionSubsystem::RegisterExtensionAsData(const FGameplayTag& ExtensionPointTag, UObject* ContextObject, UObject* Data, int32 Priority)
{
	// ExtensionPointTag(Slot)이 Invalid
	if (ExtensionPointTag.IsValid())
	{
		return FUIExtensionHandle();
	}

	// WidgetClass 유무
	if (IsValid(Data) == false)
	{
		return FUIExtensionHandle();
	}

	FExtensionList& List = ExtensionMap.FindOrAdd(ExtensionPointTag);

	// 새 FUIExtension 추가
	TSharedPtr<FUIExtension>& Entry = List.Add_GetRef(MakeShared<FUIExtension>());
	Entry->ExtensionPointTag = ExtensionPointTag;
	Entry->ContextObject = ContextObject;
	Entry->Data = Data;
	Entry->Priority = Priority;

	return FUIExtensionHandle(this, Entry);
}
