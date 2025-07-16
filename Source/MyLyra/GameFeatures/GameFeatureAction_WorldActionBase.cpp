// Fill out your copyright notice in the Description page of Project Settings.

#include "GameFeatureAction_WorldActionBase.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(GameFeatureAction_WorldActionBase)

/**
 * UE_INLINE_GENERATED_CPP_BY_NAME(...)
 * UHT가 자동으로 생성한 클래스별 정적 등록 및 리플렉션 코드를 손쉽게 include 하도록 해주는 매크로
 * 대형 프로젝트의 컴파일 효율과 유지 보수성을 모두 높여줌
 */

void UGameFeatureAction_WorldActionBase::OnGameFeatureActivating(FGameFeatureActivatingContext& Context)
{
	// 월드 순회
	for (const FWorldContext& WorldContext : GEngine->GetWorldContexts())
	{
		// 앞서, ExperienceManagerComponent에서 GameFeatureAction을 활성화하면서, Context에 World를 넣어줌
		// - 이를 통해 적용할 대상인지 판단
		if (Context.ShouldApplyToWorldContext(WorldContext))
		{
			// WorldActionBase의 Interface인 AddToWorld 호출
			AddToWorld(WorldContext, Context);
		}
	}
}