// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "../CurrsorPlayerState.h"
#include "CurrsorActionComponent.generated.h"


class ACurrsorPlayerController;
class ACurrsorPlayerState;
class ACurrsorCharacter;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CURRSOR_API UCurrsorActionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCurrsorActionComponent();

	void Initialize(ACurrsorCharacter* InPlayer, ACurrsorPlayerState* InState, ACurrsorPlayerController* InController);

	UFUNCTION()
	void Move(const FInputActionValue& Value);

	UFUNCTION()
	void UpdateRotationBasedOnInput(float DeltaTime);

	UFUNCTION()
	bool TryStartAttack();

	UFUNCTION()
	void AttackCompleted();

	// UFUNCTION()
	// void DashStarted();
	void SetMovementSpeed(float NewSpeed);
	
	UFUNCTION()
	float GetMovementSpeed() const;

	// 旋转调整控制方法
	UFUNCTION(BlueprintCallable, Category = "Rotation Control")
	void SetRotationAdjustmentEnabled(bool bEnabled);
	
	UFUNCTION(BlueprintCallable, Category = "Rotation Control")
	bool IsRotationAdjustmentEnabled() const;

	// 重置旋转调整为默认启用状态（便捷方法）
	UFUNCTION(BlueprintCallable, Category = "Rotation Control")
	void ResetRotationAdjustment();

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Player")
	TObjectPtr<ACurrsorCharacter> CurrsorPlayer;
	
	UPROPERTY(BlueprintReadOnly, Category = "Player")
	TObjectPtr<ACurrsorPlayerState> CurrsorPlayerState;

	UPROPERTY(BlueprintReadOnly, Category = "Player")
	TObjectPtr<ACurrsorPlayerController> CurrsorPlayerController;

	UPROPERTY(BlueprintReadOnly, Category = "Config|Dash", meta = (AllowPrivateAccess = "true"))
	float CurrentMovementVector = 0.0f;
	
	UPROPERTY(BlueprintReadOnly, Category = "Config|Movement", meta = (AllowPrivateAccess = "true"))
	float MovementSpeed = 600.0f;

	// 旋转调整开关控制变量
	UPROPERTY(BlueprintReadWrite, Category = "Config|Rotation", meta = (AllowPrivateAccess = "true"))
	bool bRotationAdjustmentEnabled = true;
};
