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
이 단계는 "이 맵에서는 이 캐릭터와 이 규친(GAS)로 플레이한다"는 데이터를 로드하는 과정
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
3. **최종 PlayerCharacter 스폰 (다이어그램의 27번의 실제 의미)
  - PlayerController는 PlayerState에 저장된 PawnData를 확인
  - GameMode의 SpawnPlayerPawn (또는 RestartPlayerAtPlayerStart)을 호출하여 PawnData에 정의된 **최종 PlayerCharacter**를 스폰하고, 임시 폰을 파괴한 뒤 새 캐릭터에 빙의(Possess)

## 요약
1. **접속** : GameMode가 PlayerController와 PlayerState를 만든다. (일단 스펙테이터로 입장)
2. **로드** : GameState가 이 맵의 Experience (캐릭터 데이터 PawnData 포함)를 로드.
3. **데이터 설정** : 로드가 끝나면 PlayerState가 PawnData를 받아 저장.
4. **스폰** : GameMode가 모든 준비가 끝난 것을 확인하고, PlayerState의 PawnData를 기반으로 진짜 PlayerCharacter를 스폰시킨다.

이처럼 접속과 데이터 로드를 분리하는 것이 모듈형 게임플레이의 핵심이며, 이를 통해 유연한 게임 모드를 만들 수 있다.







