// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "BaseState.generated.h"

UENUM(BlueprintType)
enum class ECharacterState : uint8
{
	Idle        UMETA(DisplayName = "Idle"),
	Walk        UMETA(DisplayName = "Walk"),
	Run         UMETA(DisplayName = "Run"),
	Jump        UMETA(DisplayName = "Jump"),
	Fall        UMETA(DisplayName = "Fall"),
	Attack      UMETA(DisplayName = "Attack"),
	RunAttack   UMETA(DisplayName = "RunAttack"),
	Dash        UMETA(DisplayName = "Dash"),

	Hurt        UMETA(DisplayName = "Hurt"),
	Dead        UMETA(DisplayName = "Dead"),
};

/**
 * 
 */
UCLASS()
class CURRSOR_API ABaseState : public APlayerState
{
	GENERATED_BODY()

public:
	virtual void Tick(float DeltaSeconds) override;
	
	// 状态更新
	UFUNCTION(BlueprintCallable, Category = "Player State")
	void UpdateState();

	// 状态变更核心逻辑
	UFUNCTION(BlueprintCallable, Category = "Player State")
	void ChangeState(ECharacterState NewState);

	// 获取当前状态
	UFUNCTION(BlueprintPure, Category = "Player State")
	ECharacterState GetCurrentState() const { return CurrentState; }
	
	UFUNCTION(BlueprintPure, Category = "Player State")
	bool IsDead() const { return CurrentState == ECharacterState::Dead; }

	UFUNCTION(BlueprintPure, Category = "Player State")
	bool IsHurt() const { return CurrentState == ECharacterState::Hurt; }
	
	UFUNCTION(BlueprintPure, Category = "Player State")
	bool IsFalling() const;

protected:

	// 状态标志
	UPROPERTY(VisibleInstanceOnly, Category = "Player State")
	bool bIsDashing = false;

	UPROPERTY(VisibleInstanceOnly, Category = "Player State")
	bool bIsAttacking = false;

	UPROPERTY(VisibleInstanceOnly, Category = "Player State")
	bool bIsJumping = false;

	UPROPERTY(VisibleInstanceOnly, Category = "Player State")
	bool bIsWalk = false;
	
	UPROPERTY(VisibleAnywhere, Category = "Player State")
	ECharacterState CurrentState = ECharacterState::Idle;

	// 阈值
	UPROPERTY(EditDefaultsOnly, Category = "Player State|Thresholds")
	float WalkThreshold = 10.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Player State|Thresholds")
	float RunThreshold = 700.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Player State|Thresholds")
	float RunAttackThreshold = 10.0f;

	// 状态进入/退出函数
	void OnEnterDash(ECharacterState PreviousState);
	void OnExitDash();
    
	void OnEnterAttack(ECharacterState PreviousState);
	void OnExitAttack();

	void OnEnterWalk(ECharacterState PreviousState);
	void OnExitWalk();

};