// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseState.h"

void ABaseState::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	UpdateState();
}

void ABaseState::UpdateState()
{
	ECharacterState NewState = CurrentState;
	
	static FVector CurrentVelocity = GetOwner()->GetVelocity();

	// 优先级：Dead > Hurt > Dash > Attack > Jump/Fall > 移动 > Idle
	if (IsDead()) {
		NewState = ECharacterState::Dead;
	}
	else if (IsHurt()) {
		NewState = ECharacterState::Hurt;
	}
	else if (bIsDashing) {
		NewState = ECharacterState::Dash;
	} 
	else if (bIsAttacking) {
		// 攻击状态下，忽略移动输入
		NewState = ECharacterState::Attack;
	}
	else if (bIsJumping) {
		NewState = ECharacterState::Jump;
	}
	else if (IsFalling()) {
		NewState = ECharacterState::Fall;
	}
	else if (CurrentVelocity.Size() > RunThreshold) {
		NewState = ECharacterState::Run;
	}
	else if (CurrentVelocity.Size() > WalkThreshold || bIsWalk) {
		NewState = ECharacterState::Walk;
	}
	else {
		NewState = ECharacterState::Idle;
	}

	// 状态变化时触发逻辑
	if (NewState != CurrentState) {
		ChangeState(NewState);
	}
}

bool ABaseState::IsFalling() const
{
	if (!Owner) return false;
	return Owner->GetVelocity().Z < 0;
}

void ABaseState::ChangeState(ECharacterState NewState)
{
	// 退出旧状态逻辑
	switch (CurrentState)
	{
	case ECharacterState::Dash:
		OnExitDash();
		break;
	case ECharacterState::Attack:
	case ECharacterState::RunAttack:
		OnExitAttack();
		break;
	case ECharacterState::Jump:
		break;
	case ECharacterState::Fall:
		break;
	case ECharacterState::Run:
		break;
	case ECharacterState::Walk:
		OnExitWalk();
		break;
	case ECharacterState::Idle:
		break;
	default: ;
	}

	// 更新当前状态
	ECharacterState PreviousState = CurrentState;
	CurrentState = NewState;

	// 进入新状态逻辑
	switch (CurrentState)
	{
	case ECharacterState::Dash:
		OnEnterDash(PreviousState);
		break;
	case ECharacterState::Attack:
		OnEnterAttack(PreviousState);
		break;
	case ECharacterState::RunAttack:
		break;
	case ECharacterState::Jump:
		break;
	case ECharacterState::Fall:
		break;
	case ECharacterState::Run:
		break;
	case ECharacterState::Walk:
		OnEnterWalk(PreviousState);
		break;
	case ECharacterState::Idle:
		break;
	default: ;
	}
}

// 状态进入/退出函数实现
void ABaseState::OnEnterDash(ECharacterState PreviousState)
{
	// 冲刺开始逻辑
	//UE_LOG(LogTemp, Log, TEXT("Entering Dash state from %d"), PreviousState);
}

void ABaseState::OnExitDash()
{
	// 冲刺结束逻辑
	//UE_LOG(LogTemp, Log, TEXT("Exiting Dash state"));
}

void ABaseState::OnEnterAttack(ECharacterState PreviousState)
{
	// 攻击开始逻辑
	//UE_LOG(LogTemp, Log, TEXT("Entering Attack state from %d"), PreviousState);
}

void ABaseState::OnExitAttack()
{
	// 攻击结束逻辑
	//UE_LOG(LogTemp, Log, TEXT("Exiting Attack state"));
}

void ABaseState::OnEnterWalk(ECharacterState PreviousState)
{
	// 行走开始逻辑
	//UE_LOG(LogTemp, Log, TEXT("Entering Walk state from %d"), PreviousState);
}

void ABaseState::OnExitWalk()
{
	// 行走结束逻辑
	//UE_LOG(LogTemp, Log, TEXT("Exiting Walk state"));
}

// 其他状态进入/退出函数类似实现...
// 这里省略了RunAttack, Jump, Fall, Run, Walk, Idle的状态函数实现