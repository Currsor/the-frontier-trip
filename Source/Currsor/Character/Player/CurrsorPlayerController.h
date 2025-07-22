// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CurrsorPlayerController.generated.h"

class UCurrsorMovementComponent;
class ACurrsorCharacter;
struct FInputActionValue;
class ACurrsorPlayerState;

class UInputMappingContext;
class UInputAction;

/**
 * 玩家控制器类
 */
UCLASS()
class CURRSOR_API ACurrsorPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	virtual void Tick(float DeltaTime) override;
	
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
	TObjectPtr<UCurrsorMovementComponent> MovementComponent;

	// 输入动作
	UPROPERTY(EditDefaultsOnly, Category = "Enhanced Input")
	TObjectPtr<UInputAction> MoveAction = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Enhanced Input")
	TObjectPtr<UInputAction> JumpAction = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Enhanced Input")
	TObjectPtr<UInputAction> DashAction = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Enhanced Input")
	TObjectPtr<UInputAction> AttackAction = nullptr;

	// 输入处理函数
	UFUNCTION()
	void Move(const FInputActionValue& Value);

	// UFUNCTION()
	// void JumpStarted();
	// UFUNCTION()
	// void JumpCompleted();

	UFUNCTION()
	void DashStarted();
	UFUNCTION()
	void DashCompleted();

	// UFUNCTION()
	// void AttackTriggered();

	// 设置和获取移动速度
	UFUNCTION(BlueprintCallable, Category = "Movement")
	void SetMovementSpeedFromTS(float NewSpeed);

	UFUNCTION(BlueprintCallable, Category = "Movement")
	float GetMovementSpeedFromTS() const;
};
