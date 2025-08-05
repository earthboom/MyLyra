// Fill out your copyright notice in the Description page of Project Settings.

#include "MyLyraHeroComponent.h"
#include "MyLyraPawnData.h"
#include "MyLyraPawnExtensionComponent.h"
#include "PlayerMappableInputConfig.h"
#include "MyLyra/Input/MyLyraInputComponent.h"
#include "MyLyra/Input/MyLyraMappableConfigPair.h"
#include "EnhancedInputSubsystems.h"
#include "Components/GameFrameworkComponentManager.h"
#include "MyLyra/MyLyraGameplayTags.h"
#include "MyLyra/MyLyraLogChannels.h"
#include "MyLyra/AbilitySystem/MyLyraAbilitySystemComponent.h"
#include "MyLyra/Camera/MyLyraCameraComponent.h"
#include "MyLyra/Player/MyLyraPlayerController.h"
#include "MyLyra/Player/MyLyraPlayerState.h"

/** FeatureName 정의 */
const FName UMyLyraHeroComponent::NAME_ActorFeatureName("Hero");

/** InputConfig의 GameFeatureAction 활성화 ExtensionEvent 이름 */
const FName UMyLyraHeroComponent::NAME_BindInputsNow("BindInputsNow");

UMyLyraHeroComponent::UMyLyraHeroComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;
}

void UMyLyraHeroComponent::OnRegister()
{
	Super::OnRegister();

	// 정상적인 Actor에 등록되어 있는지를 확인
	{
		if (GetPawn<APawn>() == nullptr)
		{
			UE_LOG(LogMyLyra, Error, TEXT("This component has been added to a BP whose base class is not a Pawn!"));
			return;
		}
	}

	// GameFrameworkComponentManager에 InitState 사용을 위해 등록 진행
	// - 등록은 상속 받았던 IGameFrameworkInitStateInterface 함수 RegisterInitStateFeature() 활용
	RegisterInitStateFeature();
}

void UMyLyraHeroComponent::BeginPlay()
{
	Super::BeginPlay();

	// PawnExtensionComponent 에 대해서(PawnExtension Feature) OnActorInitStateChanged() 관찰하도록 (Observing)
	BindOnActorInitStateChanged(UMyLyraPawnExtensionComponent::NAME_ActorFeatureName, FGameplayTag(), false);

	// InitState_Spawned 로 초기화
	ensure(TryToChangeInitState(FMyLyraGameplayTags::Get().InitState_Spawned));

	// ForceUpdate 진행
	CheckDefaultInitialization();
}

void UMyLyraHeroComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnregisterInitStateFeature();

	Super::EndPlay(EndPlayReason);
}

FName UMyLyraHeroComponent::GetFeatureName() const
{
	return NAME_ActorFeatureName;
}

void UMyLyraHeroComponent::OnActorInitStateChanged(const FActorInitStateChangedParams& Params)
{
	const FMyLyraGameplayTags& InitTags = FMyLyraGameplayTags::Get();

	if (Params.FeatureName == UMyLyraPawnExtensionComponent::NAME_ActorFeatureName)
	{
		// MyLyraPawnExtensionComponent는 DataInitialized 상태 변화 관찰 후, MyLyraHeroComponent도 DataInitialized 상태로 변경
		// - CanChangeInitState 확인
		if (Params.FeatureState == InitTags.InitState_DataInitialized)
		{
			CheckDefaultInitialization();
		}
	}
}

bool UMyLyraHeroComponent::CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const
{
	check(Manager);

	const FMyLyraGameplayTags& InitTags = FMyLyraGameplayTags::Get();
	APawn* Pawn = GetPawn<APawn>();
	AMyLyraPlayerState* MyLyraPS = GetPlayerState<AMyLyraPlayerState>();

	// InitState_Spawned 초기화
	if (CurrentState.IsValid() == false && DesiredState == InitTags.InitState_Spawned)
	{
		// Pawn이 잘 세팅만 되어 있으면 바로 Spawned 로 넘어감
		if (IsValid(Pawn))
		{
			return true;
		}
	}

	// Spawned -> DataAvailable
	if (CurrentState == InitTags.InitState_Spawned && DesiredState == InitTags.InitState_DataAvailable)
	{
		if (IsValid(MyLyraPS) == false)
		{
			return false;
		}
		return true;
	}

	// DataAvailable -> DataInitialized
	if (CurrentState == InitTags.InitState_DataAvailable && DesiredState == InitTags.InitState_DataInitialized)
	{
		// PawnExtensionComponent 가 DataInitialized될 때까지 기다림 ( == 모든 Feature Component가 DataAvailable인 상태 )
		return MyLyraPS && Manager->HasFeatureReachedInitState(Pawn, UMyLyraPawnExtensionComponent::NAME_ActorFeatureName, InitTags.InitState_DataInitialized);
	}

	// DataInitialized -> GameplayReady
	if (CurrentState == InitTags.InitState_DataInitialized && DesiredState == InitTags.InitState_GameplayReady)
	{
		return true;
	}

	return false;
}

void UMyLyraHeroComponent::HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState)
{
	// IGameFrameworkInitStateInterface::HandleChangeInitState(Manager, CurrentState, DesiredState);

	const FMyLyraGameplayTags& InitTags = FMyLyraGameplayTags::Get();

	// DataAvaliable -> DataInitialized 단계
	if (CurrentState == InitTags.InitState_DataAvailable && DesiredState == InitTags.InitState_DataInitialized)
	{
		APawn* Pawn = GetPawn<APawn>();
		AMyLyraPlayerState* MyLyraPS = GetPlayerState<AMyLyraPlayerState>();
		if (ensure(Pawn && MyLyraPS) == false)
		{
			return;
		}

		const bool bIsLocallyControlled = Pawn->IsLocallyControlled();
		const UMyLyraPawnData* PawnData = nullptr;
		UMyLyraPawnExtensionComponent* PawnExtComp = UMyLyraPawnExtensionComponent::FindPawnExtensionComponent(Pawn);
		if (IsValid(PawnExtComp))
		{
			PawnData = PawnExtComp->GetPawnData<UMyLyraPawnData>();

			// DataInitialized 단계까지 오면, Pawn이 Controller에 Possess되어 준비된 상태
			// - InitAbilityActorInfo 호출로 AvatarActor 재설정이 필요
			PawnExtComp->InitializeAbilitySystem(MyLyraPS->GetMyLyraAbilitySystemComponent(), MyLyraPS);
		}

		// Input, Camera 핸들링


		// const bool bIsLocallyControlled = Pawn->IsLocallyControlled();
		// const UMyLyraPawnData* PawnData = nullptr;
		// UMyLyraPawnExtensionComponent* PawnExtComp = UMyLyraPawnExtensionComponent::FindPawnExtensionComponent(Pawn);
		// if (PawnExtComp != nullptr)
		// {
		// 	PawnData = PawnExtComp->GetPawnData<UMyLyraPawnData>();
		// }

		if (bIsLocallyControlled && IsValid(PawnData))
		{
			// 현재 MyLyraCharater에 Attach된 CameraComponent를 찾음
			UMyLyraCameraComponent* CameraComponent = UMyLyraCameraComponent::FindCameraComponent(Pawn);
			if (IsValid(CameraComponent))
			{
				CameraComponent->DetermineCameraModeDelegate.BindUObject(this, &UMyLyraHeroComponent::DetermineCameraMode);
			}
		}

		AMyLyraPlayerController* MyLyraPC = GetController<AMyLyraPlayerController>();
		if (IsValid(MyLyraPC))
		{
			if (IsValid(Pawn->InputComponent))
			{
				InitializePlayerInput(Pawn->InputComponent);
			}
		}
	}
}

void UMyLyraHeroComponent::CheckDefaultInitialization()
{
	// 앞서 BindOnActorInitStateChanged 에서 봤듯, Hero Feature 는 Pawn Extension Feature에 종속되어 있으므로, CheckDefaultInitializationForImplementers 을 호출하지 않음
	// ContinueInitStateChain 은 앞서 PawnExtensionComponent와 같음
	const FMyLyraGameplayTags& InitTags = FMyLyraGameplayTags::Get();
	static const TArray<FGameplayTag> StateChain = {InitTags.InitState_Spawned, InitTags.InitState_DataAvailable, InitTags.InitState_DataInitialized, InitTags.InitState_GameplayReady};
	ContinueInitStateChain(StateChain);
}

PRAGMA_DISABLE_OPTIMIZATION

TSubclassOf<UMyLyraCameraMode> UMyLyraHeroComponent::DetermineCameraMode() const
{
	const APawn* Pawn = GetPawn<APawn>();
	if (IsValid(Pawn) == false)
	{
		return nullptr;
	}

	UMyLyraPawnExtensionComponent* PawnExtComp = UMyLyraPawnExtensionComponent::FindPawnExtensionComponent(Pawn);
	if (IsValid(PawnExtComp))
	{
		const UMyLyraPawnData* PawnData = PawnExtComp->GetPawnData<UMyLyraPawnData>();
		if (IsValid(PawnData))
		{
			return PawnData->DefaultCameraMode;
		}
	}

	return nullptr;
}

PRAGMA_ENABLE_OPTIMIZATION

void UMyLyraHeroComponent::InitializePlayerInput(UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);

	const APawn* Pawn = GetPawn<APawn>();
	if (IsValid(Pawn) == false)
	{
		return;
	}

	// Local Player 접근을 위함
	const APlayerController* PC = GetController<APlayerController>();
	check(PC);

	// EnhancedInputLocalPlayerSubsystem 접근을 위함
	const ULocalPlayer* LP = PC->GetLocalPlayer();
	check(LP);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(Subsystem);

	// EnhancedInputLocalPlayerSubsystem에 MappingContext를 비워줌
	Subsystem->ClearAllMappings();

	// PawnExtensionComponent -> PawnData -> InputConfig Valid 체크
	const UMyLyraPawnExtensionComponent* PawnExtComp = UMyLyraPawnExtensionComponent::FindPawnExtensionComponent(Pawn);
	if (IsValid(PawnExtComp))
	{
		const UMyLyraPawnData* PawnData = PawnExtComp->GetPawnData<UMyLyraPawnData>();
		if (IsValid(PawnData))
		{
			const UMyLyraInputConfig* InputConfig = PawnData->InputConfig;
			if (IsValid(InputConfig))
			{
				const FMyLyraGameplayTags& GameplayTags = FMyLyraGameplayTags::Get();

				// HeroComponent 가지고 있는 Input Mapping Context를 순회, EnhancedInputLocalPlayerSubsystem에 추가
				for (const FMyLyraMappableConfigPair& Pair : DefaultInputConfigs)
				{
					if (Pair.bShouldActivateAutomatically)
					{
						FModifyContextOptions Options = {};
						Options.bIgnoreAllPressedKeysUntilRelease = false;

						// 내부적으로 Input Mapping Context를 추가
						const UPlayerMappableInputConfig* Config = Pair.Config.LoadSynchronous();
						for (const TPair<TObjectPtr<UInputMappingContext>, int>& MappingContextPair : Config->GetMappingContexts())
						{
							const UInputMappingContext* MappingContext = MappingContextPair.Key;
							const int32 Priority = MappingContextPair.Value;
							Subsystem->AddMappingContext(MappingContext, Priority, Options);
						}
					}
				}

				UMyLyraInputComponent* MyLyraIC = CastChecked<UMyLyraInputComponent>(PlayerInputComponent);
				{
					{
						TArray<uint32> BindHandles;
						MyLyraIC->BindAbilityAction(InputConfig, this, &ThisClass::Input_AbilityInputTagPressed, &ThisClass::Input_AbilityInputTagReleased, BindHandles);
					}

					// InputTag_Move, InputTag_look_Mouse에 대해 각각 Input_Move(), Input_LookMouse() 멤버 함수에 바인딩
					// - 바인딩한 이후, input 이벤트에 따라 멤버 함수가 트리거 됨
					MyLyraIC->BindNativeAction(InputConfig, GameplayTags.InputTag_Move, ETriggerEvent::Triggered, this, &ThisClass::Input_Move, false);
					MyLyraIC->BindNativeAction(InputConfig, GameplayTags.InputTag_Look_Mouse, ETriggerEvent::Triggered, this, &ThisClass::Input_LookMove, false);
				}
			}
		}
	}

	// GameFeatureAction_AddInputConfig의 HandlePawnExtension 콜백 함수 전달
	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(const_cast<APawn*>(Pawn), NAME_BindInputsNow);
}

void UMyLyraHeroComponent::Input_Move(const FInputActionValue& InputActionValue)
{
	APawn* Pawn = GetPawn<APawn>();
	AController* Controller = Pawn ? Pawn->GetController() : nullptr;

	if (IsValid(Controller))
	{
		const FVector2D Value = InputActionValue.Get<FVector2D>();
		const FRotator MovementRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);

		if (Value.X != 0.0f)
		{
			// Left / Right -> X 값에 들어있음
			// MovementDirection은 현재 카메라의 Rightvector를 의미 (World-Space)
			const FVector MovementDirection = MovementRotation.RotateVector(FVector::RightVector);

			// AddMovementInput
			// - 내부적으로 MovementDirection * Value.X를 MovementComponent에 적용 (더하기)
			Pawn->AddMovementInput(MovementDirection, Value.X);
		}

		if (Value.Y != 0.0f)
		{
			const FVector MovementDirection = MovementRotation.RotateVector(FVector::ForwardVector);
			Pawn->AddMovementInput(MovementDirection, Value.Y);
		}
	}
}

void UMyLyraHeroComponent::Input_LookMove(const FInputActionValue& InputActionValue)
{
	APawn* Pawn = GetPawn<APawn>();
	if (IsValid(Pawn) == false)
	{
		return;
	}

	const FVector2D Value = InputActionValue.Get<FVector2D>();
	if (Value.X != 0.0f)
	{
		// X에 Yaw 값
		// - Camera에 대해 Yaw 적용
		Pawn->AddControllerYawInput(Value.X);
	}

	if (Value.Y != 0.0f)
	{
		// Y = Pitch
		double AimInversionValue = -Value.Y;
		Pawn->AddControllerPitchInput(AimInversionValue);
	}
}

void UMyLyraHeroComponent::Input_AbilityInputTagPressed(FGameplayTag InputTag)
{
	const APawn* Pawn = GetPawn<APawn>();
	if (IsValid(Pawn))
	{
		const UMyLyraPawnExtensionComponent* PawnExtComp = UMyLyraPawnExtensionComponent::FindPawnExtensionComponent(Pawn);
		if (IsValid(PawnExtComp))
		{
			UMyLyraAbilitySystemComponent* MyLyraASC = PawnExtComp->GetMyLyraAbilitySystemComponent();
			if (IsValid(MyLyraASC))
			{
				MyLyraASC->AbilityInputTagPressed(InputTag);
			}
		}
	}
}

void UMyLyraHeroComponent::Input_AbilityInputTagReleased(FGameplayTag InputTag)
{
	const APawn* Pawn = GetPawn<APawn>();
	if (IsValid(Pawn))
	{
		const UMyLyraPawnExtensionComponent* PawnExtComp = UMyLyraPawnExtensionComponent::FindPawnExtensionComponent(Pawn);
		if (IsValid(PawnExtComp))
		{
			UMyLyraAbilitySystemComponent* MyLyraASC = PawnExtComp->GetMyLyraAbilitySystemComponent();
			if (IsValid(MyLyraASC))
			{
				MyLyraASC->AbilityInputTagReleased(InputTag);
			}
		}
	}
}
