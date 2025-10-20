// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Currsor/Character/Component/BaseState.h"
#include "Currsor/Interface/ICombatInterface.h"
#include "Currsor/Interface/IDamageable.h"
#include "GameFramework/PlayerController.h"
#include "CurrsorPlayerController.generated.h"

class UCurrsorActionComponent;
class ACurrsorCharacter;
struct FInputActionValue;
class ACurrsorPlayerState;
class UGameSystemManager;
class UAttackSystemComponent;
class UStateManagerComponent;

class UInputMappingContext;
class UInputAction;
class UBattleAreaTeleportComponent;

/**
 * 玩家控制器类
 */
UCLASS()
class CURRSOR_API ACurrsorPlayerController : public APlayerController, public ICombatInterface, public IDamageable
{
	GENERATED_BODY()
public:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void SetupInputComponent() override;
	virtual void Tick(float DeltaTime) override;

	//~ Begin ICombatStateInterface
	virtual void AttackEnd_Implementation() override;
	virtual void AttackHitboxOn_Implementation() override;
	virtual void AttackHitboxOff_Implementation() override;
	//~ End ICombatStateInterface

	//~ Begin IDamageable
	// virtual void ApplyDamage_Implementation(float DamageAmount) override;
	//~ End IDamageable

	// 公共访问方法
	UFUNCTION(BlueprintCallable, Category = "Components")
	UCurrsorActionComponent* GetPlayerActionComponent() const { return PlayerActionComponent; }

	// 输入映射上下文切换
	UFUNCTION(BlueprintCallable, Category = "Input")
	void SwitchInputMappingContext(bool bUseCombatInput);

	// 退出战斗区域
	UFUNCTION(BlueprintCallable, Category = "Battle")
	void ExitBattleArea();
	
protected:
	// 输入映射上下文
	UPROPERTY(EditDefaultsOnly, Category = "Enhanced Input")
	TObjectPtr<UInputMappingContext> InputMappingContext = nullptr;

	// 战斗输入映射上下文
	UPROPERTY(EditDefaultsOnly, Category = "Enhanced Input")
	TObjectPtr<UInputMappingContext> CombatInputMappingContext = nullptr;

	// 属性
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	float CurrentMovementVector;

	UPROPERTY(BlueprintReadOnly, Category = "Player")
	TObjectPtr<ACurrsorCharacter> CurrsorPlayer = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "State")
	TObjectPtr<ACurrsorPlayerState> CurrsorPlayerState = nullptr;

	// 组件
	UPROPERTY(BlueprintReadOnly, Category = "Components")
	TObjectPtr<UCurrsorActionComponent> PlayerActionComponent;

	UPROPERTY(BlueprintReadOnly, Category = "Components")
	TObjectPtr<ACurrsorPlayerState> PlayerStateComponent;

	// 系统管理器
	UPROPERTY(BlueprintReadOnly, Category = "Systems")
	TObjectPtr<UGameSystemManager> GameSystemManager;

	UPROPERTY(BlueprintReadOnly, Category = "Systems")
	TObjectPtr<UAttackSystemComponent> AttackSystem;

	UPROPERTY(BlueprintReadOnly, Category = "Systems")
	TObjectPtr<UStateManagerComponent> StateManager;

	// 输入动作
	UPROPERTY(EditDefaultsOnly, Category = "Enhanced Input")
	TObjectPtr<UInputAction> MoveAction = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Enhanced Input")
	TObjectPtr<UInputAction> JumpAction = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Enhanced Input")
	TObjectPtr<UInputAction> DashAction = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Enhanced Input")
	TObjectPtr<UInputAction> AttackAction = nullptr;

	// 输入模式控制
	UPROPERTY(BlueprintReadOnly, Category = "Input")
	bool bIsInCombatInputMode = false;

	// 战斗区域传送组件引用
	UPROPERTY(BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBattleAreaTeleportComponent> BattleAreaTeleportComponent;

	// 调试日志开关
	UPROPERTY(EditDefaultsOnly, Category = "Debug")
	bool bEnableInputDebugLogging = true;
	
private:
	// 输入处理函数
	UFUNCTION()
	void Move(const FInputActionValue& Value);

	UFUNCTION()
	void MoveStarted();

	UFUNCTION()
	void MoveCompleted();

	UFUNCTION()
	void AttackTriggered();

	UFUNCTION()
	void AttackStarted();

	UFUNCTION()
	void AttackCanceled();

	UFUNCTION()
	void AttackCompleted();

	// UFUNCTION()
	// void JumpStarted();
	// UFUNCTION()
	// void JumpCompleted();

	// UFUNCTION()
	// void DashStarted();
	// UFUNCTION()
	// void DashCompleted();

	// UFUNCTION()
	// void AttackTriggered();
};
