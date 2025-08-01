// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CurrsorPlayerState.h"
#include "PaperZDCharacter.h"
#include "CurrsorCharacter.generated.h"

class UBoxComponent;
class ACurrsorPlayerState;
class UCurrsorCameraComponent;
class USpringArmComponent;
/**
 * 玩家角色类
 */
UCLASS()
class CURRSOR_API ACurrsorCharacter : public APaperZDCharacter
{
	GENERATED_BODY()
	
public:
	ACurrsorCharacter();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	// 弹簧臂组件
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> SpringArmComponent;

	// 相机组件
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCurrsorCameraComponent> CameraComponent;

	// 攻击碰撞盒
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Attack, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBoxComponent> AttackHitbox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<ACurrsorPlayerState> CurrsorPlayerState = Cast<ACurrsorPlayerState>(GetPlayerState());
	
	// 上一帧的弹簧臂长度
	UPROPERTY(VisibleInstanceOnly)
	float LastArmLength;

	// 上一帧的碰撞状态
	UPROPERTY(VisibleInstanceOnly)
	bool LastCollisionState;
};
