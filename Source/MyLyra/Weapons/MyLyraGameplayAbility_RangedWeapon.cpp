// Fill out your copyright notice in the Description page of Project Settings.

#include "MyLyraGameplayAbility_RangedWeapon.h"
#include "AbilitySystemComponent.h"
#include "MyLyraRangedWeaponInstance.h"
#include "MyLyra/AbilitySystem/MyLyraGameplayAbilityTargetData_SingleTargetHit.h"
#include "MyLyra/Physics/MyLyraCollisionChannels.h"

UMyLyraGameplayAbility_RangedWeapon::UMyLyraGameplayAbility_RangedWeapon(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UMyLyraGameplayAbility_RangedWeapon::StartRangedWeaponTargeting()
{
	/**
	 * ActorInfo는 AbilitySet에서 GiveAbility() 호출로 설정됨
	 * - UGameplayAbility::OnGibeAbility()에서 SetCurrentActorInfo()에서 설정됨
	 * - AbilitySYstemComponent::GiveAbility에서 OnGiveAbility에서 호출됨
	 * - MyLyraAbilitySet::GiveToAbililtySystem()에서 호출됨
	 */
	check(CurrentActorInfo);

	AActor* AvatarActor = CurrentActorInfo->AvatarActor.Get();
	check(AvatarActor);

	UAbilitySystemComponent* MyASC = CurrentActorInfo->AbilitySystemComponent.Get();
	check(MyASC);

	/** Lyra는 샷건 처리와 같은 탄착 처리를 생략, 권총으로 진행 (아래의 로직은 간단한 버전) */

	// 총알 궤적의 Hit 정보를 계산
	TArray<FHitResult> FoundHits;
	PerformLocalTargeting(FoundHits);

	// GameplayAbilityTargetData는 Server / Client 간 Ability의 공유 데이터로 이해하면 됨
	// - 현 프로젝트는 싱글 플레이이므로 Ability의 데이터로 생각하면 됨 (현 프로젝트엔 큰 의미가 없음)
	FGameplayAbilityTargetDataHandle TargetData;
	TargetData.UniqueId = 0;

	if (FoundHits.Num() > 0)
	{
		// Cartridge란 일반 권총인 경우, 탄약에 하나의 총알이 들어있지만, 샷건의 경우, 탄약에 여러 총알이 있다, ** 탄약을 카트리지로 생각 **
		const int32 CartridgeID = FMath::Rand();
		for (const FHitResult& FoundHit : FoundHits)
		{
			// AbilityTargetData에 SingleTargetHit 정보를 담는다.
			// - 참고로 TargetData.Add()의 경우, SharedPtr에 넣기 때문에 여기서 new는 크게 신경쓰지 않아도 됨
			FMyLyraGameplayAbilityTargetData_SingleTargetHit* NewTargetData = new FMyLyraGameplayAbilityTargetData_SingleTargetHit();
			NewTargetData->HitResult = FoundHit;
			NewTargetData->CartridgeID = CartridgeID;
			TargetData.Add(NewTargetData);
		}
	}

	// 가공된 AbilityTargetData가 준비되었으므로, OnTargetDataReadyCallback을 호출
	OnTargetDataReadyCallback(TargetData, FGameplayTag());
}

void UMyLyraGameplayAbility_RangedWeapon::PerformLocalTargeting(TArray<FHitResult>& OutHits)
{
	APawn* const AvatarPawn = Cast<APawn>(GetAvatarActorFromActorInfo());

	UMyLyraRangedWeaponInstance* WeaponData = GetWeaponInstance();
	if (IsValid(AvatarPawn) && AvatarPawn->IsLocallyControlled() && IsValid(WeaponData))
	{
		FRangedWeaponFiringInput InputData;
		InputData.WeaponData = WeaponData;
		InputData.bCanPlayBulletFX = true;

		const FTransform TargetTransform = GetTargetingTransform(AvatarPawn, EMyLyraAbilityTargetingSource::CameraTowardsFocus);

		// 언리얼은 ForwardVector가 (1, 0, 0) 즉 EAxis::X
		// - GetUnitAxis()를 살펴보자
		InputData.AimDir = TargetTransform.GetUnitAxis(EAxis::X);
		InputData.StartTrace = TargetTransform.GetTranslation();
		InputData.EndAim = InputData.StartTrace + InputData.AimDir * WeaponData->MaxDamageRange;

#if 0
		{
			static float DebugThickness = 2.0f;
			DrawDebugLine(GetWorld(), InputData.StartTrace, InputData.StartTrace + (InputData.AimDir * 100.0f), FColor::Yellow, false, DebugThickness);
		}
#endif

		TraceBulletsCartridge(InputData, OutHits);
	}
}

FTransform UMyLyraGameplayAbility_RangedWeapon::GetTargetingTransform(APawn* SourcePawn, EMyLyraAbilityTargetingSource Source)
{
	check(SourcePawn);
	check(Source == EMyLyraAbilityTargetingSource::CameraTowardsFocus);

	// 아래 로직은 CameraTowardsFocus만 추출한 로직 ( 완전한 로직은 Lyra 참고 )

	AController* Controller = SourcePawn->GetController();
	if (IsValid(Controller) == false)
	{
		return FTransform();
	}

	double FocalDistance = 1024.0f;
	FVector FocalLoc;
	FVector CamLoc;
	FRotator CamRot;

	// PlayerController로 부터, 현재 카메라의 Location, Rotation 정보를 획득
	APlayerController* PC = Cast<APlayerController>(Controller);
	check(PC);
	PC->GetPlayerViewPoint(CamLoc, CamRot);

	// 사격 방향
	FVector AimDir = CamRot.Vector().GetSafeNormal();
	FocalLoc = CamLoc + (AimDir * FocalDistance);

	// WeaponLoc이 아닌 Pawn의
	const FVector WeaponLoc = GetWeaponTargetingSourceLocation();
	FVector FinalCamLoc = FocalLoc + (((WeaponLoc - FocalLoc) | AimDir) * AimDir);

#if 0
	{
		// WeaponLoc (사실상 ActorLoc)
		DrawDebugPoint(GetWorld(), WeaponLoc, 10.0f, FColor::Red, false, 60.0f);
		// CamLoc
		DrawDebugPoint(GetWorld(), CamLoc, 10.0f, FColor::Yellow, false, 60.0f);
		// FinalCamLoc
		DrawDebugPoint(GetWorld(), FinalCamLoc, 10.0f, FColor::Magenta, false, 60.0f);

		// (WeaponLoc - FocalLoc)
		DrawDebugLine(GetWorld(), FocalLoc, WeaponLoc, FColor::Yellow, false, 60.0f, 0, 2.0f);
		// (AimDir)
		DrawDebugLine(GetWorld(), CamLoc, FocalLoc, FColor::Blue, false, 60.0f, 0, 2.0f);

		// Project Direction Line
		DrawDebugLine(GetWorld(), WeaponLoc, FinalCamLoc, FColor::Red, false, 60.0f, 0, 2.0f);
	}
#endif

	// Camera -> Focus 계산 완료
	return FTransform(CamRot, FinalCamLoc);
}

FVector UMyLyraGameplayAbility_RangedWeapon::GetWeaponTargetingSourceLocation() const
{
	// Weapon 위치가 아닌 Pawn의 위치를 가져옴 ( Lyra에서도 미구현이라 되어 있음 )
	APawn* const AvatarPawn = Cast<APawn>(GetAvatarActorFromActorInfo());
	check(AvatarPawn);

	const FVector SourceLoc = AvatarPawn->GetActorLocation();
	return SourceLoc;
}

void UMyLyraGameplayAbility_RangedWeapon::TraceBulletsCartridge(const FRangedWeaponFiringInput& InputData, TArray<FHitResult>& OutHits)
{
	UMyLyraRangedWeaponInstance* WeaponData = InputData.WeaponData;
	check(WeaponData);

	// MaxDamageRange를 고려해, EndTrace 정의
	const FVector BulletDir = InputData.AimDir;
	const FVector EndTrace = InputData.StartTrace + (BulletDir * WeaponData->MaxDamageRange);

	// HitLocation의 초기화 값으로 ENdTrace로 설정
	FVector HitLocation = EndTrace;

	// 총알 하나 Trace 진행
	// - Lyra의 경우, 샷건과 같은 Cartridge에 여러 개의 총알이 있을 경우를 처리하기 위해 for loop를 활용, 복수개 Bullet을 Trace함
	TArray<FHitResult> AllImpacts;
	FHitResult Impact = DoSingleBulletTrace(InputData.StartTrace, EndTrace, WeaponData->BulletTraceWeaponRadius, /**bIsSimulated*/ false, /**out*/ AllImpacts);

	const AActor* HitActor = Impact.GetActor();
	if (IsValid(HitActor))
	{
		if (AllImpacts.Num() > 0)
		{
			OutHits.Append(AllImpacts);
		}

		HitLocation = Impact.ImpactPoint;
	}

	// OutHits가 적어도 하나가 존재하도록, EndTrace를 활용해, OutHits에 추가해 줌
	if (OutHits.Num() == 0)
	{
		if (Impact.bBlockingHit == false)
		{
			Impact.Location = EndTrace;
			Impact.ImpactPoint = EndTrace;
		}

		OutHits.Add(Impact);
	}
}

int32 FindFirstPawnHitResult(const TArray<FHitResult>& HitResults)
{
	for (int32 Idx = 0; Idx < HitResults.Num(); ++Idx)
	{
		const FHitResult& CurHitResult = HitResults[Idx];
		if (CurHitResult.HitObjectHandle.DoesRepresentClass(APawn::StaticClass()))
		{
			return Idx;
		}
		else
		{
			AActor* HitActor = CurHitResult.HitObjectHandle.FetchActor();

			// 한 단계 AttachParent에 Actor가 Pawn이라면?
			// - 보통 복수개 단계로 AttachParent를 하지 않으므로, AttachParent 대상이 APawn이라 생각할 수도?
			if (IsValid(HitActor) && (HitActor->GetAttachParentActor() != nullptr) && (Cast<APawn>(HitActor->GetAttachParentActor()) != nullptr))
			{
				return Idx;
			}
		}
	}
	return INDEX_NONE;
}

FHitResult UMyLyraGameplayAbility_RangedWeapon::DoSingleBulletTrace(const FVector& StartTrace, const FVector& EndTrace, float SweepRadius, bool bIsSimulated, TArray<FHitResult>& OutHits) const
{
	FHitResult Impact;

	// 우선 SweepRadius 없이 한 번 Trace 진행 (SweepTrace는 무겁기 때문)
	// - FindFirstPawnHitResult()를 여러번 Trace 진행을 막기 위해, OutHits를 확인해 APawn 충돌 정보 있으면 더 이상 Trace 하지 않음
	if (FindFirstPawnHitResult(OutHits) == INDEX_NONE)
	{
		Impact = WeaponTrace(StartTrace, EndTrace, /**SweepRadius*/0.0f, bIsSimulated, /**Out*/OutHits);
	}

	if (FindFirstPawnHitResult(OutHits) == INDEX_NONE)
	{
		// 만약 SweepRadius가 0보다 크면, 0.0일 때 대비 충돌 가능성이 커지므로 한번 더 Trace 진행
		if (SweepRadius > 0.0f)
		{
			// SweepHits에 Trace의 OutHits 정보를 저장
			TArray<FHitResult> SweepHits;
			Impact = WeaponTrace(StartTrace, EndTrace, SweepRadius, bIsSimulated, SweepHits);

			// Sphere Trace로 진행한 결과인 SweepHits를 검색하여, Pawn이 있는가 검색
			const int32 FirstPawnIdx = FindFirstPawnHitResult(SweepHits);
			if (SweepHits.IsValidIndex(FirstPawnIdx))
			{
				// 만약 있으면, SweepHits를 FirstPawnIdx까지 순회하며, bBlockingHit와 기준 OutHits에 없을 경우 체크
				bool bUseSweepHits = true;
				for (int32 idx = 0; idx < FirstPawnIdx; ++idx)
				{
					const FHitResult& CurHitResult = SweepHits[idx];

					TFunction<bool(const FHitResult&)> Pred = [&CurHitResult](const FHitResult& Other)-> bool
					{
						return Other.HitObjectHandle == CurHitResult.HitObjectHandle;
					};

					// OutHits에 있다면... SweepHits를 OutHits로 업데이트하지 않음 (아마 충돌했던 정보가 있어서 그런듯?) (early-out용)
					// - OutHits에 bBlockingHits가 SweepHits로 있음을 알게되었음
					if (CurHitResult.bBlockingHit && OutHits.ContainsByPredicate(Pred))
					{
						bUseSweepHits = false;
						break;
					}
				}

				// SweepHits
				if (bUseSweepHits)
				{
					OutHits = SweepHits;
				}
			}
		}
	}

	return Impact;
}

FHitResult UMyLyraGameplayAbility_RangedWeapon::WeaponTrace(const FVector& StartTrace, const FVector& EndTrace, float SweepRadius, bool bIsSimulated, TArray<FHitResult>& OutHitResults) const
{
	TArray<FHitResult> HitResults;

	// Complex Geometry로 Trace를 진행하며, AvatarActor를 AttachParent를 가지는 오브젝트와의 충돌은 무시
	FCollisionQueryParams TraceParams(SCENE_QUERY_STAT(WeaponTrac), /*bTraceComplex*/true, /*IgnoreActor=*/GetAvatarActorFromActorInfo());
	TraceParams.bReturnPhysicalMaterial = true;

	// AvatarActor에 부착된 Actors를 찾아 IgnoredActors에 추가
	AddAdditionalTraceIgnoreActors(TraceParams);

	// Weapon 관련 Collision Channel로 Trace 진행
	const ECollisionChannel TraceChannel = DetermineTraceChannel(TraceParams, bIsSimulated);
	if (SweepRadius > 0.0f)
	{
		GetWorld()->SweepMultiByChannel(HitResults, StartTrace, EndTrace, FQuat::Identity, TraceChannel, FCollisionShape::MakeSphere(SweepRadius), TraceParams);
	}
	else
	{
		GetWorld()->LineTraceMultiByChannel(HitResults, StartTrace, EndTrace, TraceChannel, TraceParams);
	}

	FHitResult Hit(ForceInit);
	if (HitResults.Num() > 0)
	{
		// HitResults 중에 중복(같은) Object의 HitResult 정보를 제거
		for (FHitResult& CurHitResult : HitResults)
		{
			TFunction<bool(const FHitResult&)> Pred = [&CurHitResult](const FHitResult& Other)-> bool
			{
				return Other.HitObjectHandle == CurHitResult.HitObjectHandle;
			};

			if (OutHitResults.ContainsByPredicate(Pred) == false)
			{
				OutHitResults.Add(CurHitResult);
			}
		}

		// Hit의 가장 마지막 값을 Impact로 저장
		Hit = OutHitResults.Last();
	}
	else
	{
		// Hit의 결과 값을 기본 값으로 캐싱
		Hit.TraceStart = StartTrace;
		Hit.TraceEnd = EndTrace;
	}

	return Hit;
}

ECollisionChannel UMyLyraGameplayAbility_RangedWeapon::DetermineTraceChannel(FCollisionQueryParams& TraceParams, bool bIsSimulated) const
{
	return MyLyra_TraceChannel_Weapon;
}

void UMyLyraGameplayAbility_RangedWeapon::AddAdditionalTraceIgnoreActors(FCollisionQueryParams& TraceParams) const
{
	AActor* Avatar = GetAvatarActorFromActorInfo();
	if (IsValid(Avatar))
	{
		TArray<AActor*> AttachedActors;

		/**
		 * GetAttachedActors
		 * - 해당 함수는 Recursively하게 모든 Actors를 추출
		 */
		Avatar->GetAttachedActors(AttachedActors);

		TraceParams.AddIgnoredActors(AttachedActors);
	}
}

void UMyLyraGameplayAbility_RangedWeapon::OnTargetDataReadyCallback(const FGameplayAbilityTargetDataHandle& InData, FGameplayTag ApplicationTag)
{
	UAbilitySystemComponent* MyASC = CurrentActorInfo->AbilitySystemComponent.Get();
	check(MyASC);

	const FGameplayAbilitySpec* AbilitySpec = MyASC->FindAbilitySpecFromHandle(CurrentSpecHandle);
	if (AbilitySpec)
	{
		// 현재 Stack에서 InData에서 지금 Local로 OwnerShip을 가져옮
		FGameplayAbilityTargetDataHandle LocalTargetDataHandle(MoveTemp(const_cast<FGameplayAbilityTargetDataHandle&>(InData)));

		// CommitAbility 호출로 GE ( GameplayEffect )를 처리
		if (CommitAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo))
		{
			// OnRangeWeaponTargetDataReady BP 노드 호출
			// - 여기서 GC(GameplayCueNotify) 처리도 함
			OnRangeWeaponTargetDataReady(LocalTargetDataHandle);
		}
		else
		{
			// CommitAbility가 실패하였으면, EndAbility BP Node 호출
			K2_EndAbility();
		}
	}
}

UMyLyraRangedWeaponInstance* UMyLyraGameplayAbility_RangedWeapon::GetWeaponInstance()
{
	return Cast<UMyLyraRangedWeaponInstance>(GetAssociatedEquipment());
}
