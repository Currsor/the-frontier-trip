// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Currsor/Interface/CombatInterface.h"
#include "GameFramework/PlayerController.h"
#include "CurrsorPlayerController.generated.h"

class UCurrsorActionComponent;
class ACurrsorCharacter;
struct FInputActionValue;
class ACurrsorPlayerState;

class UInputMappingContext;
class UInputAction;

/**
 * 玩家控制器类
 */
UCLASS()
class CURRSOR_API ACurrsorPlayerController : public APlayerController, public ICombatInterface
{
	GENERATED_BODY()
public:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	virtual void Tick(float DeltaTime) override;

	//~ Begin ICombatStateInterface
	virtual void AttackEnd_Implementation() override;
	//~ End ICombatStateInterface

	// Debug
	UPROPERTY(EditDefaultsOnly, Category = "Debug")
	TSubclassOf<UUserWidget> DebugUIClass; // Debug UI的蓝图类

	UPROPERTY()
	UUserWidget* DebugUIInstance; // Debug UI的实例

	// 显示/隐藏Debug UI
	UFUNCTION()
	void ToggleDebugUI();
	
protected:
	// 输入映射上下文
	UPROPERTY(EditDefaultsOnly, Category = "Enhanced Input")
	TObjectPtr<UInputMappingContext> InputMappingContext = nullptr;

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

	// 输入动作
	UPROPERTY(EditDefaultsOnly, Category = "Enhanced Input")
	TObjectPtr<UInputAction> MoveAction = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Enhanced Input")
	TObjectPtr<UInputAction> JumpAction = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Enhanced Input")
	TObjectPtr<UInputAction> DashAction = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Enhanced Input")
	TObjectPtr<UInputAction> AttackAction = nullptr;
	
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
