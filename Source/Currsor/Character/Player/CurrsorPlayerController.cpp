// Fill out your copyright notice in the Description page of Project Settings.


#include "CurrsorPlayerController.h"

#include "CurrsorCharacter.h"
#include "CurrsorPlayerState.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"
#include "Component/CurrsorActionComponent.h"
#include "Currsor/System/CurrsorGameInstance.h"
#include "Currsor/System/GameSystemManager.h"
#include "Currsor/System/Components/AttackSystemComponent.h"
#include "Currsor/System/Components/StateManagerComponent.h"
#include "Currsor/System/Components/BattleAreaTeleportComponent.h"

void ACurrsorPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();
    if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
    {
        EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ACurrsorPlayerController::Move);
        EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Started, this, &ACurrsorPlayerController::MoveStarted);
        EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &ACurrsorPlayerController::MoveCompleted);
        // EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACurrsorPlayerController::JumpStarted);
        // EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACurrsorPlayerController::JumpCompleted);
        // EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Triggered, this, &ACurrsorPlayerController::DashStarted);
        // EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Completed, this, &ACurrsorPlayerController::DashCompleted);
        EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &ACurrsorPlayerController::AttackTriggered);
        EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &ACurrsorPlayerController::AttackStarted);
        EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Canceled, this, &ACurrsorPlayerController::AttackCanceled);
        EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Completed, this, &ACurrsorPlayerController::AttackCompleted);

    }
}

void ACurrsorPlayerController::BeginPlay()
{
    Super::BeginPlay();
    if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
    {
        Subsystem->AddMappingContext(InputMappingContext, 0);
    }
    
    CurrsorPlayer = Cast<ACurrsorCharacter>(GetCharacter());
    CurrsorPlayerState = GetPlayerState<ACurrsorPlayerState>();
    CurrsorPlayerState = Cast<ACurrsorPlayerState>(CurrsorPlayerState);

    // 初始化游戏系统管理器
    GameSystemManager = UGameSystemManager::GetInstance(GetWorld());
    if (GameSystemManager && !GameSystemManager->IsInitialized())
    {
        GameSystemManager->Initialize(GetWorld());
    }

    // 获取系统引用
    if (GameSystemManager)
    {
        AttackSystem = GameSystemManager->GetAttackSystem();
        StateManager = GameSystemManager->GetStateManager();
        
        // 获取战斗区域传送组件
        // 获取BattleAreaTeleportComponent（通过AttackSystem）
        if (AttackSystem)
        {
            BattleAreaTeleportComponent = AttackSystem->GetBattleAreaTeleportComponent();
        }
        
        if (bEnableInputDebugLogging)
        {
            UE_LOG(LogTemp, Log, TEXT("PlayerController已初始化系统组件"));
        }
    }

    // 依赖项通过 Initialize 注入
    PlayerActionComponent = NewObject<UCurrsorActionComponent>(this);
    PlayerActionComponent->Initialize(CurrsorPlayer, CurrsorPlayerState, this);

    PlayerStateComponent = GetPlayerState<ACurrsorPlayerState>();
}

void ACurrsorPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
}

void ACurrsorPlayerController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    // TODO: 这里可以添加其他逻辑，比如更新状态或处理输入
    PlayerActionComponent->UpdateRotationBasedOnInput(DeltaTime);
    // PlayerActionComponent->UpdateDash(DeltaTime);
}

void ACurrsorPlayerController::Move(const FInputActionValue& Value)
{
    if (PlayerActionComponent) PlayerActionComponent->Move(Value);
}

void ACurrsorPlayerController::MoveStarted()
{
    if (!CurrsorPlayerState->ShouldMove()) return;
    if(PlayerStateComponent) PlayerStateComponent->SetWalking(true);
}

void ACurrsorPlayerController::MoveCompleted()
{
    if(PlayerStateComponent) PlayerStateComponent->SetWalking(false);
}

void ACurrsorPlayerController::AttackTriggered()
{
	UE_LOG(LogTemp, Log, TEXT("连续攻击"));
}

void ACurrsorPlayerController::AttackStarted()
{
	UE_LOG(LogTemp, Log, TEXT("攻击"));
    CurrsorPlayerState -> SetAttackKey(true);
    
    // 使用新的攻击系统
    if (AttackSystem && AttackSystem->CanAttack(CurrsorPlayer))
    {
        AttackSystem->StartAttack(CurrsorPlayer, TEXT("Normal"));
    }
    
    if (PlayerActionComponent) PlayerActionComponent->TryStartAttack();
}

void ACurrsorPlayerController::AttackCanceled()
{
	UE_LOG(LogTemp, Log, TEXT("攻击结束"));
    CurrsorPlayerState -> SetAttackKey(false);
}

void ACurrsorPlayerController::AttackCompleted()
{
	UE_LOG(LogTemp, Log, TEXT("连续攻击结束"));
    CurrsorPlayerState -> SetAttackKey(false);
    
}

void ACurrsorPlayerController::AttackEnd_Implementation()
{
    ICombatInterface::AttackEnd_Implementation();

    // 使用新的攻击系统
    if (AttackSystem)
    {
        AttackSystem->EndAttack(CurrsorPlayer);
    }

    if (!PlayerStateComponent->GetAttackKey()) PlayerActionComponent->AttackCompleted();
}

void ACurrsorPlayerController::AttackHitboxOn_Implementation()
{
    ICombatInterface::AttackHitboxOn_Implementation();

    PlayerStateComponent->SetAttackNum(1.f);

    CurrsorPlayer->SetHitboxCollision(true);
}

void ACurrsorPlayerController::AttackHitboxOff_Implementation()
{
    ICombatInterface::AttackHitboxOff_Implementation();

    PlayerStateComponent->SetAttackNum(0.f);
    
    CurrsorPlayer->SetHitboxCollision(false);
}

void ACurrsorPlayerController::ExitBattleArea()
{
    if (!BattleAreaTeleportComponent)
    {
        if (bEnableInputDebugLogging)
        {
            UE_LOG(LogTemp, Warning, TEXT("BattleAreaTeleportComponent为空，无法退出战斗区域"));
        }
        return;
    }
    
    if (!CurrsorPlayer)
    {
        if (bEnableInputDebugLogging)
        {
            UE_LOG(LogTemp, Warning, TEXT("CurrsorPlayer为空，无法退出战斗区域"));
        }
        return;
    }
    
    BattleAreaTeleportComponent->ExitBattleArea(CurrsorPlayer);
    
    if (bEnableInputDebugLogging)
    {
        UE_LOG(LogTemp, Log, TEXT("PlayerController请求退出战斗区域"));
    }
}

void ACurrsorPlayerController::SwitchInputMappingContext(bool bUseCombatInput)
{
    if (bIsInCombatInputMode == bUseCombatInput) return;

    if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
    {
        // 移除当前IMC
        if (bIsInCombatInputMode && CombatInputMappingContext)
        {
            Subsystem->RemoveMappingContext(CombatInputMappingContext);
        }
        else if (!bIsInCombatInputMode && InputMappingContext)
        {
            Subsystem->RemoveMappingContext(InputMappingContext);
        }

        // 添加新的IMC
        if (bUseCombatInput && CombatInputMappingContext)
        {
            Subsystem->AddMappingContext(CombatInputMappingContext, 1);
            bIsInCombatInputMode = true;
            
            if (bEnableInputDebugLogging)
            {
                UE_LOG(LogTemp, Log, TEXT("切换到战斗输入模式"));
            }
        }
        else if (!bUseCombatInput && InputMappingContext)
        {
            Subsystem->AddMappingContext(InputMappingContext, 1);
            bIsInCombatInputMode = false;
            
            if (bEnableInputDebugLogging)
            {
                UE_LOG(LogTemp, Log, TEXT("切换到普通输入模式"));
            }
        }
    }
}

// TODO: 这里可以添加其他输入处理函数
// void ACurrsorPlayerController::JumpStarted() { PlayerActionComponent->JumpStarted(); }
// void ACurrsorPlayerController::JumpCompleted() { PlayerActionComponent->JumpCompleted(); }
// void ACurrsorPlayerController::DashStarted() { PlayerActionComponent->DashStarted(); }
// void ACurrsorPlayerController::DashCompleted() { PlayerActionComponent->DashCompleted(); }
// void ACurrsorPlayerController::AttackTriggered() { AttackComponent->AttackTriggered(); }
// void ACurrsorPlayerController::AttackEnd_Implementation() { AttackComponent->AttackEnd(); }