# 구현할 클래스의 관계도

<img width="1912" height="982" alt="image" src="https://github.com/user-attachments/assets/0390cf36-6fda-40b2-b6ee-069acaaf64cb" />

# PlayerCharacter가 Spawn되는 과정

<img width="1024" height="624" alt="image" src="https://github.com/user-attachments/assets/7b27533f-ddd5-4926-ad07-ac4eaddf0d30" />

두 개의 큰 흐름이 비동기적으로 진행되다가 마지막에 만나게 됨
1. **A흐름 (접속)** : 플레이어가 서버에 접속하고 PlayerController와 PlayerState를 받는 과정 (주로 GameMode 담당)
2. **B흐름 (Experience 로드)** : 이 월드에서 플레이어가 어떤 캐릭터와 규칙(PawnData)을 가질지 **경험(Experience)**을 로드하는 과정 (주로 GameState 담당)

## 1단계 : 플레이어 접속 및 컨트롤러 생성 (GameMode 역할)
이 단계는 플레이어가 게임에 "들어와서" 조작할 수 있는 PlayerController 를 받는 과정
1. **플레이어 접속 시작 (1 ~ 11)**
  - SpawnPlayer (혹은 Login) 이벤트가 발생하면, GameMode가 이 플레이어를 처리할 책임을 가진다.
  - GameMode는 GameState의 ExperienceManagerComponent가 준비되었는지 확인 (8-10)
2. **컨트롤러 및 상태 생성 (12 ~15, 30)**
  - GameMode가 Login 로직을 실행하여, 이 플레이어를 위한 **PlayerController**를 생성 (12)
  - PlayerController는 생성되자마자 자신의 **PlayerState**를 생성하여 서버에 등록 (15, 30)
3. **임시 폰(Pawn) 스폰 (20 ~ 29)**
  - GameMode는 HandleStartingNewPlayer를 호출하여 플레이어가 게임을 시작할 준비 (20)
  - 플레이어 시작 지점(PlayerStart)을 찾는다. (22)
  - **SpawnDefaultPawnAtTransform**을 호출하여 **임시 폰(Pawn)**  을 스폰 (26)
    - [중요] 이 폰은 아직 최종 PlayerCharacter가 아닐 수 있다. (예: 스펙테이터(Spectator) 폰)
    - 이 폰은 PlayerController에 빙의(Possess)된다. (29)
  
  **현재 상태** : 플레이어는 게임 월드에 PlayerController와 PlayerState, 그리고 (아마도) 임시 폰을 가지고 접속했지만, 아직 자신이 어떤 캐릭터가 되어야 하는지 모른다.

## 2단계 : 게임 경험(Experience) 로드 (GameState 역할)
이 단계는 "이 맵에서는 이 캐릭터와 이 규칙(GAS)으로 플레이한다"는 데이터를 로드하는 과정
1. **경험(Experience) 설정 (31 ~ 33)**
  - 별도의 이벤트(예: Host/Join 또는 OnMatchAssignmentGiven)가 GameMode에게 "이제 이 경험을 로드하라"고 알린다. (31)
  - GameMode는 GameState에 있는 ExperienceManagerComponent에게 해당 경험을 설정하라고 명령 (33)
2. **비동기 로드 시작 (34 ~ 35)**
  - ExperienceManagerComponent가 StartExperienceLoad를 호출
  - 이 경험에 필요한 모든 것(예: PawnData, 게임플레이 어빌리티(GAS) 세트, UI 등)을 비동기(Async)로 로드하기 시작
  
  **현재 상태** : 시스템이 백그라운드에서 플레이어가 사용할 캐릭터(PawnData)와 어빌리티 정보를 불러오고 있다.

## 3단계 : 최종 스폰 (데이터와 플레이어의 만남)
두 흐름이 만나 실제 PlayerCharacter가 스폰되는 마지막 단계
1. **경험 로드 완료 (36 ~ 39)**
  - 비동기 로드가 완료되면, ExperienceManagerComponent가 OnExperienceLoadCompleted를 호출 (36)
  - GameState는 OnExperienceLoaded_Broadcast라는 델리게이트(신호)를 맵 전체에 브로드캐스트한다. "경험 로드 완료!" (37)
  - **PlayerState**가 이 신호를 듣고(38), 로드된 경험의 PawnData를 자신의 변수에 저장 (39)
2. **GameMode의 최종 확인 (40 ~ 42)**
  - GameMode 역시 이 신호를 듣는다. (40)
  - 이제 GameMode는 "플레이어도 접속해 있고(1단계), 경험(데이터)도 로드되었음(2단계)"을 안다.
  - GameMode는 PlayerCanRestart 같은 함수를 통해 PlayerController에게 "이제 진짜 캐릭터를 스폰해도 된다"고 알린다. (42)
3. **최종 PlayerCharacter 스폰 (다이어그램의 27번의 실제 의미)**
  - PlayerController는 PlayerState에 저장된 PawnData를 확인
  - GameMode의 SpawnPlayerPawn (또는 RestartPlayerAtPlayerStart)을 호출하여 PawnData에 정의된 **최종 PlayerCharacter**를 스폰하고, 임시 폰을 파괴한 뒤 새 캐릭터에 빙의(Possess)

## 요약
1. **접속** : GameMode가 PlayerController와 PlayerState를 만든다. (일단 스펙테이터로 입장)
2. **로드** : GameState가 이 맵의 Experience (캐릭터 데이터 PawnData 포함)를 로드.
3. **데이터 설정** : 로드가 끝나면 PlayerState가 PawnData를 받아 저장.
4. **스폰** : GameMode가 모든 준비가 끝난 것을 확인하고, PlayerState의 PawnData를 기반으로 진짜 PlayerCharacter를 스폰시킨다.

이처럼 접속과 데이터 로드를 분리하는 것이 모듈형 게임플레이의 핵심이며, 이를 통해 유연한 게임 모드를 만들 수 있다.


# Module과 Plugin 그리고 Project
### Module
- Module은 하나의 dll 단위로 라이브러리로 생각하자
- .h / .cpp 를 **묶는 하나의 최소 단위로 생각**할 수 있다.
- Unreal Engine은 Module 정의를 위해 필요한 구성 요소는 아래와 같다.
    - 우선 Module의 이름을 myLyraFeature라고 하자
    1. MyLyraFeature.Build.cs
    2. FMyLyraFeatureModule.h / .cpp
      - IMPLEMENT_PRIMARY_GAME_MODULE
- 참고로, **Module은 단독으로 Unreal Engine에서 구성할 수 없다.**
    - Plugin 혹은 Project에 종속되어 활성화 되어야 한다!

### Plugin [Code]
- Unreal Engine을 **확장하는 단위**
    - uni-direction한 참조 구조를 가지게 된다.

<img width="486" height="625" alt="image" src="https://github.com/user-attachments/assets/12676f6c-3c73-40fa-a337-4acc572381fe" />

- 반대로, **UnrealEngine에서 Plugin을 참조할 수 없다!**
    - 보통 Unreal Engine에서도 Experimental한 Plugin은 따로 외부 Plugin으로 구성한 이후, 충분히 검증되면(Productio-Ready), 내부 Module로 통합한다.

- **알아두면 좋은 특징들**
    - 복수 개의 Module로 구성:
      - **.h/.cpp로 구성할 수 없음!**
    - 복수 개의 Plugin을 참조 가능
    - description 파일로 .uplugin 을 참고
    - 독립적인 Content 폴더 구성 가능
- Plugin의 구성요소
  우선 Plugin 이름을 MyLyraPlugin 이라 가정
  1. MyLyraPlugin.uplugin
  2. 1 개 이상의 Module 들 **(없어도 됨)**

### Project
- Unreal Engine의 활용 대상이 되는 프로젝트
- **알아두면 좋은 특징들**
    - 복수 개의 Module로 구성
        - Plugin과 마찬가지로 **.h / .cpp 로 구성할 수 없음!**
    - 복수 개의 Plugin 참조 및 구성 가능
        - 하나의 uproject에 대해 여러 개의 Plugin을 포함도 가능하다는 것을 알 수 있다.
        - 물론 구성하지 않은 **외부의 Plugin도 참조 가능**

<img width="453" height="765" alt="image" src="https://github.com/user-attachments/assets/e89a0673-fa04-474c-b712-f080e7153d5b" />

  - description 파일로 .uproject를 참조
- Project의 구성요소
  Project의 이름을 MyLyra 라고 가정
  1. MyLyra.uproject
  2. MyLyra[Editor|Game|Server|Client].Target.cs:
       - **빌드 대상을 나누고 싶은 개수대로 추가 가능**
  3. 1개 이상 Module들

### Plugin [Content]
- Unreal Engine은 Blueprint가 있고, 이를 통해 코드 없이 에셋으로 Plugin을 구성할 수 있다.
- 앞서 언급했뜻, Plugin은 독립적인 Content 구조를 가져갈 수 있다.
    - **해당 경로에 Blueprint와 같은 로직 관련 에셋을 통해 Unreal Engine을 확장하는 구조를 가져갈 수 있다.**
- 이 경우, **Editor를 통해 Plugin 생성을 추천**

#### Unreal Engine 과 LyraStarterGame을 통해 아래 구조 확인

<img width="966" height="788" alt="image" src="https://github.com/user-attachments/assets/6e888d11-1627-40d1-be76-af8ba2b377e8" />
<img width="711" height="669" alt="image" src="https://github.com/user-attachments/assets/d9d82553-7f0b-422b-a76a-62869ab87cc7" />

1. Unreal Engine과 Project(uproject)는 Module과 Plugin으로 구성되어 있다.
2. Plugin은 Module로 구성되어 있다.
    - Unreal Engine의 Plugin인 Bridge는 2개의 Module로 구성되어 있다.
         - Bridge, MegascansPlugin
         - 여기서, Plugin은 복수개의 Module을 포함하는 것을 확인할 수 있다.
    - LyraStarterGame Project에도 똑같이 Plugin이 Module로 포함되게 된다.

#### 실제로 Plugin과 Module은 Build.cs 분석 및 uplugin과 uproject 파일에 대한 속성 값들을 이해해야 제대로 활용 가능
- Circular Referencing 에 대한 이해 필요하지만 이 프로젝트의 목적과 벗어난 부분
- Module <-> Plugin, Project에 대해 이해만 목적으로 한다

---

## GameMode 관계도
- AGameState
- AMyLyraPlayerController
- AMyLyraPlayerState
- AMyLyraCharacter

<img width="3347" height="864" alt="image" src="https://github.com/user-attachments/assets/95d1fba8-ddbe-4e74-946d-6b1e6ba66f02" />










