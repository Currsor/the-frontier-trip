// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperZDCharacter.h"
#include "Currsor/Interface/IDamageable.h"
#include "Engine/DataTable.h"
#include "Engine/UserDefinedStruct.h"
#include "CurrsorCharacter.generated.h"

class ACurrsorGameMode;
class UBoxComponent;
class ACurrsorPlayerState;
class UCurrsorCameraComponent;
class USpringArmComponent;
class UHealthComponent;
class UGameSystemManager;
class UAttackSystemComponent;
class UBattleAreaTeleportComponent;
/**
 * 玩家角色类
 */
UCLASS()
class CURRSOR_API ACurrsorCharacter : public APaperZDCharacter, public IDamageable
{
	GENERATED_BODY()
	
public:
	ACurrsorCharacter();

	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetHitboxCollision(bool bCollision);

	//~ Begin IDamageable Interface
	virtual void ApplyDamage_Implementation(float DamageAmount, AActor* DamageInstigator, const FHitResult& HitResult) override;
	//~ End IDamageable Interface

	// PlayerState相关方法
	UFUNCTION(BlueprintPure, Category = "Player")
	APlayerState* GetCurrsorPlayerState() const;

	// 生命值相关方法
	UFUNCTION(BlueprintPure, Category = "Health")
	float GetHealth() const;

	UFUNCTION(BlueprintPure, Category = "Health")
	float GetMaxHealth() const;

	UFUNCTION(BlueprintPure, Category = "Health")
	bool IsDead() const;

	// Mana
	UFUNCTION(BlueprintPure, Category = "Mana")
	int32 GetMana() const;

	UFUNCTION(BlueprintPure, Category = "Mana")
	int32 GetMaxMana() const;

	UFUNCTION(BlueprintCallable, Category = "Mana")
	void UseMana(int32 Amount);

	// 区域ID相关方法（通过GameState管理）
	UFUNCTION(BlueprintPure, Category = "BattleArea")
	int32 GetCurrentAreaID() const;

	UFUNCTION(BlueprintCallable, Category = "BattleArea")
	void SetCurrentAreaID(int32 NewAreaID);

	UFUNCTION(BlueprintPure, Category = "BattleArea")
	bool HasValidAreaID() const;

	// 旋转调整控制方法
	UFUNCTION(BlueprintCallable, Category = "Movement")
	void SetRotationAdjustmentEnabled(bool bEnabled);

	UFUNCTION(BlueprintPure, Category = "Movement")
	bool IsRotationAdjustmentEnabled() const;

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void ResetRotationAdjustment();

	// 相机切换方法
	UFUNCTION(BlueprintCallable, Category = "Camera")
	void SwitchToPlayerCamera();

	UFUNCTION(BlueprintCallable, Category = "Camera")
	void SwitchToBattleCamera(class UBillboardComponent* CameraBillboard);

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
	
	// 系统管理器
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Systems", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UGameSystemManager> GameSystemManager;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Systems", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAttackSystemComponent> AttackSystem;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Systems", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBattleAreaTeleportComponent> BattleAreaTeleportComponent;


	
	// 上一帧的弹簧臂长度
	UPROPERTY(VisibleInstanceOnly)
	float LastArmLength;

	// 上一帧的碰撞状态
	UPROPERTY(VisibleInstanceOnly)
	bool LastCollisionState;

	TObjectPtr<ACurrsorGameMode> GameMode;
};
