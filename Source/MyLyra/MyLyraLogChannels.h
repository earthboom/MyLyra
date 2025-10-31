// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Containers/UnrealString.h"
#include "Logging/LogMacros.h"

DECLARE_LOG_CATEGORY_EXTERN(LogMyLyra, Log, All);

/**
 * 2025.10.31
 * DECLARE_LOG_CATEGORY_EXTERN
 * "우리 프로젝트 전역에서 사용할 'LogMyLyra'라는 이름의 로그 카테고리를 선언하겠다" 라고 알리는 역할!
 * - 헤더 파일에서 로그 카테고리를 선언(Declare)한다.
 * - EXTERN 키워드는 "이 로그 카테고리의 실체(정의)는 다른 어딘가에 있다."는 것을 의미
 * - 이 선언 덕에, 이 헤더 파일을 포함(#include)한 모든 다른 파일에서 LogMyLyra 카테고리를 인식하고 사용할 수 있게 된다.
 */