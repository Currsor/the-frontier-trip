// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CurrsorPlayerState.h"
#include "PaperZDCharacter.h"
#include "Currsor/Interface/IDamageable.h"
#include "CurrsorCharacter.generated.h"

class ACurrsorGameMode;
class UBoxComponent;
class ACurrsorPlayerState;
class UCurrsorCameraComponent;
class USpringArmComponent;
class UHealthComponent;
/**
 * 玩家角色类
 */
UCLASS()
class CURRSOR_API ACurrsorCharacter : public APaperZDCharacter, public IDamageable
{
	GENERATED_BODY()
	
public:
	ACurrsorCharacter();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetHitboxCollision(bool bCollision);

	//~ Begin IDamageable Interface
	virtual void ApplyDamage_Implementation(float DamageAmount, AActor* DamageInstigator, const FHitResult& HitResult) override;
	//~ End IDamageable Interface

	// 生命值相关方法
	UFUNCTION(BlueprintPure, Category = "Health")
	float GetHealth() const;

	UFUNCTION(BlueprintPure, Category = "Health")
	float GetMaxHealth() const;

	UFUNCTION(BlueprintPure, Category = "Health")
	bool IsDead() const;

private:
	// 弹簧臂组件
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> SpringArmComponent;

	// 相机组件
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCurrsorCameraComponent> CameraComponent;

	// 攻击碰撞盒
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attack", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBoxComponent> AttackHitbox;

	// 生命值组件
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UHealthComponent> HealthComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<ACurrsorPlayerState> CurrsorPlayerState = Cast<ACurrsorPlayerState>(GetPlayerState());
	
	// 上一帧的弹簧臂长度
	UPROPERTY(VisibleInstanceOnly)
	float LastArmLength;

	// 上一帧的碰撞状态
	UPROPERTY(VisibleInstanceOnly)
	bool LastCollisionState;

	TObjectPtr<ACurrsorGameMode> GameMode;
};
