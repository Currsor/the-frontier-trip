#include "CurrsorPlayerState.h"

#include "CurrsorCharacter.h"
#include "CurrsorPlayerController.h"

ACurrsorPlayerState::ACurrsorPlayerState()
{
    PrimaryActorTick.bCanEverTick = true;
}

void ACurrsorPlayerState::BeginPlay()
{
    Super::BeginPlay();
    
    CurrsorController = Cast<ACurrsorPlayerController>(GetOwner());
    if (!CurrsorController.IsValid()) return;
    CurrsorCharacter = Cast<ACurrsorCharacter>(GetPawn());
}

void ACurrsorPlayerState::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    UpdateState();
}

void ACurrsorPlayerState::UpdateState()
{
    EPlayerState NewState = CurrentState;

    if (!CurrsorCharacter.IsValid()) return;
    CurrsorVelocity = CurrsorCharacter->GetVelocity();

    // 优先级：Dash > Attack > Jump/Fall > 移动 > Idle
    if (IsDashing()) {
        NewState = EPlayerState::Dash;
    } 
    else if (IsAttacking()) {
        NewState = (CurrsorVelocity.Size() > 0) ? EPlayerState::RunAttack : EPlayerState::Attack;
    }
    else if (IsJumping()) {
        NewState = EPlayerState::Jump;
    }
    else if (IsFalling()) {
        NewState = EPlayerState::Fall;
    }
    else if (CurrsorVelocity.Size() > RunThreshold) {
        NewState = EPlayerState::Run;
    }
    else if (CurrsorVelocity.Size() > WalkThreshold) {
        NewState = EPlayerState::Walk;
    }
    else {
        NewState = EPlayerState::Idle;
    }

    // 状态变化时触发逻辑
    if (NewState != CurrentState) {
        ChangeState(NewState);
    }
}

void ACurrsorPlayerState::ChangeState(EPlayerState NewState)
{
    // 退出旧状态逻辑
    switch (CurrentState)
    {
    case EPlayerState::Dash:
        OnExitDash();
        break;
    case EPlayerState::Attack:
    case EPlayerState::RunAttack:
        OnExitAttack();
        break;
    case EPlayerState::Jump:
        break;
    case EPlayerState::Fall:
        break;
    case EPlayerState::Run:
        break;
    case EPlayerState::Walk:
        break;
    case EPlayerState::Idle:
        break;
    }

    // 更新当前状态
    EPlayerState PreviousState = CurrentState;
    CurrentState = NewState;

    // 进入新状态逻辑
    switch (CurrentState)
    {
    case EPlayerState::Dash:
        OnEnterDash(PreviousState);
        break;
    case EPlayerState::Attack:
        OnEnterAttack(PreviousState);
        break;
    case EPlayerState::RunAttack:
        break;
    case EPlayerState::Jump:
        break;
    case EPlayerState::Fall:
        break;
    case EPlayerState::Run:
        break;
    case EPlayerState::Walk:
        break;
    case EPlayerState::Idle:
        break;
    }
}

bool ACurrsorPlayerState::IsDashing() const
{
    // 实现你的冲刺检测逻辑
    return bIsDashing;
}

bool ACurrsorPlayerState::IsAttacking() const
{
    // 实现你的攻击检测逻辑
    return bIsAttacking;
}

bool ACurrsorPlayerState::IsJumping() const
{
    // 实现你的跳跃检测逻辑
    return bIsJumping;
}

bool ACurrsorPlayerState::IsFalling() const
{
    // 实现你的下落检测逻辑
    if (!Owner) return false;
    return Owner->GetVelocity().Z < 0;
}

// 状态进入/退出函数实现
void ACurrsorPlayerState::OnEnterDash(EPlayerState PreviousState)
{
    // 冲刺开始逻辑
    UE_LOG(LogTemp, Log, TEXT("Entering Dash state from %d"), PreviousState);
}

void ACurrsorPlayerState::OnExitDash()
{
    // 冲刺结束逻辑
    UE_LOG(LogTemp, Log, TEXT("Exiting Dash state"));
}

void ACurrsorPlayerState::OnEnterAttack(EPlayerState PreviousState)
{
    // 攻击开始逻辑
    UE_LOG(LogTemp, Log, TEXT("Entering Attack state from %d"), PreviousState);
}

void ACurrsorPlayerState::OnExitAttack()
{
    // 攻击结束逻辑
    UE_LOG(LogTemp, Log, TEXT("Exiting Attack state"));
}

// 其他状态进入/退出函数类似实现...
// 这里省略了RunAttack, Jump, Fall, Run, Walk, Idle的状态函数实现
// 实际项目中应该全部实现

void ACurrsorPlayerState::SetDashing(bool bDashing)
{
    bIsDashing = bDashing;
    UpdateState();
}

void ACurrsorPlayerState::SetAttacking(bool bAttacking)
{
    bIsAttacking = bAttacking;
    UpdateState();
}

void ACurrsorPlayerState::SetJumping(bool bJumping)
{
    bIsJumping = bJumping;
    UpdateState();
}
