#include "CurrsorPlayerState.h"

#include "CurrsorCharacter.h"
#include "CurrsorPlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"

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
}

bool ACurrsorPlayerState::CanStartAttack() const
{
    // 判断是否不在冲刺或攻击状态
    return !bIsDashing && !bIsAttacking;
}

bool ACurrsorPlayerState::ShouldMove()
{
    // 攻击或冲刺状态下不允许移动
    if (GetCurrentState() == ECharacterState::Attack || GetCurrentState() == ECharacterState::Dash) return false;
    return true;
}

bool ACurrsorPlayerState::IsGrounding() const
{
    if (!Owner) return false;
    return Cast<ACharacter>(Owner) -> GetCharacterMovement() -> IsMovingOnGround();
}

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

void ACurrsorPlayerState::SetWalking(bool bWalking)
{
    bIsWalk = bWalking;
    UpdateState();
}
