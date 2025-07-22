// Fill out your copyright notice in the Description page of Project Settings.


#include "CurrsorPlayerController.h"

#include "CurrsorCharacter.h"
#include "CurrsorPlayerState.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Component/CurrsorMovementComponent.h"

void ACurrsorPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();
    if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
    {
        EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ACurrsorPlayerController::Move);
        // EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACurrsorPlayerController::JumpStarted);
        // EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACurrsorPlayerController::JumpCompleted);
        EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Triggered, this, &ACurrsorPlayerController::DashStarted);
        EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Completed, this, &ACurrsorPlayerController::DashCompleted);
        // EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &ACurrsorPlayerController::AttackTriggered);
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

    // 依赖项通过 Initialize 注入
    MovementComponent = NewObject<UCurrsorMovementComponent>(this);
    MovementComponent->Initialize(CurrsorPlayer, CurrsorPlayerState, this);
}

void ACurrsorPlayerController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    // TODO: 这里可以添加其他逻辑，比如更新状态或处理输入
    MovementComponent->UpdateRotationBasedOnInput(DeltaTime);
    // MovementComponent->UpdateDash(DeltaTime);
}

void ACurrsorPlayerController::Move(const FInputActionValue& Value)
{
    if (MovementComponent) MovementComponent->Move(Value);
}
// TODO: 这里可以添加其他输入处理函数
// void ACurrsorPlayerController::JumpStarted() { MovementComponent->JumpStarted(); }
// void ACurrsorPlayerController::JumpCompleted() { MovementComponent->JumpCompleted(); }
void ACurrsorPlayerController::DashStarted() { MovementComponent->DashStarted(); }
void ACurrsorPlayerController::DashCompleted() { MovementComponent->DashCompleted(); }
// void ACurrsorPlayerController::AttackTriggered() { AttackComponent->AttackTriggered(); }
// void ACurrsorPlayerController::AttackEnd_Implementation() { AttackComponent->AttackEnd(); }

// 设置和获取移动速度
void ACurrsorPlayerController::SetMovementSpeedFromTS(float NewSpeed)
{
    if (MovementComponent) MovementComponent->SetMovementSpeed(NewSpeed);
    UE_LOG(LogTemp, Warning, TEXT("SetMovementSpeedFromTS: %f"), NewSpeed);
}

float ACurrsorPlayerController::GetMovementSpeedFromTS() const
{
    return MovementComponent ? MovementComponent->GetMovementSpeed() : 0.f;
}