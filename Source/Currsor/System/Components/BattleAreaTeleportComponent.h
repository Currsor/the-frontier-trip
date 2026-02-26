// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/World.h"
#include "BattleAreaTeleportComponent.generated.h"

class AAreaCollisionBox;
class ACurrsorCharacter;
class ABaseEnemy;
class ACurrsorGameState;

/**
 * 战斗区域传送组件
 * 负责处理玩家攻击敌人后的区域传送逻辑
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CURRSOR_API UBattleAreaTeleportComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UBattleAreaTeleportComponent();

protected:
	virtual void BeginPlay() override;

public:
	/**
	 * 处理战斗区域传送
	 * @param Player 玩家角色
	 * @param Enemy 被攻击的敌人
	 * @return 是否成功执行传送
	 */
	UFUNCTION(BlueprintCallable, Category = "Battle Area Teleport")
	bool ProcessBattleAreaTeleport(ACurrsorCharacter* Player, ABaseEnemy* Enemy);

	/**
	 * 传送玩家到指定位置
	 * @param Player 玩家角色
	 * @param TargetLocation 目标位置
	 * @param TargetRotation 目标旋转
	 */
	UFUNCTION(BlueprintCallable, Category = "Battle Area Teleport")
	void TeleportPlayer(ACurrsorCharacter* Player, const FVector& TargetLocation, const FRotator& TargetRotation);

	/**
	 * 传送敌人到指定位置
	 * @param Enemy 敌人角色
	 * @param TargetLocation 目标位置
	 * @param TargetRotation 目标旋转
	 */
	UFUNCTION(BlueprintCallable, Category = "Battle Area Teleport")
	void TeleportEnemy(ABaseEnemy* Enemy, const FVector& TargetLocation, const FRotator& TargetRotation);

	/**
	 * 移动相机到指定位置（已弃用，保留兼容性）
	 * @param Player 玩家角色（用于获取相机）
	 * @param TargetLocation 目标位置
	 * @param TargetRotation 目标旋转
	 */
	UFUNCTION(BlueprintCallable, Category = "Battle Area Teleport")
	void MoveCameraToPosition(ACurrsorCharacter* Player, const FVector& TargetLocation, const FRotator& TargetRotation);

	/**
	 * 切换到战斗区域相机
	 * @param Player 玩家角色
	 * @param CameraBillboard 相机Billboard组件，用于查找子相机
	 */
	UFUNCTION(BlueprintCallable, Category = "Battle Area Teleport")
	void SwitchToBattleCamera(ACurrsorCharacter* Player, class UBillboardComponent* CameraBillboard);

	/**
	 * 退出战斗区域
	 * @param Player 玩家角色
	 */
	UFUNCTION(BlueprintCallable, Category = "Battle Area Teleport")
	void ExitBattleArea(ACurrsorCharacter* Player);

	/**
	 * 切换回玩家相机
	 * @param Player 玩家角色
	 */
	UFUNCTION(BlueprintCallable, Category = "Battle Area Teleport")
	void SwitchToPlayerCamera(ACurrsorCharacter* Player);

	/**
	 * 获取区域碰撞盒
	 * @param AreaID 区域ID
	 * @return 区域碰撞盒，如果未找到返回nullptr
	 */
	UFUNCTION(BlueprintPure, Category = "Battle Area Teleport")
	AAreaCollisionBox* GetAreaCollisionBox(int32 AreaID) const;

protected:
	/**
	 * 验证传送条件
	 * @param Player 玩家角色
	 * @param Enemy 敌人角色
	 * @return 是否满足传送条件
	 */
	UFUNCTION(BlueprintPure, Category = "Battle Area Teleport")
	bool ValidateTeleportConditions(ACurrsorCharacter* Player, ABaseEnemy* Enemy) const;

	/**
	 * 获取游戏状态
	 * @return 游戏状态，如果未找到返回nullptr
	 */
	UFUNCTION(BlueprintPure, Category = "Battle Area Teleport")
	ACurrsorGameState* GetCurrsorGameState() const;

private:
	// 传送配置
	UPROPERTY(EditAnywhere, Category = "Battle Area Teleport|Config", meta = (DisplayName = "启用调试日志记录"))
	bool bEnableDebugLogging = true;

	UPROPERTY(EditAnywhere, Category = "Battle Area Teleport|Config", meta = (DisplayName = "传送延迟"))
	float TeleportDelay = 0.1f;

	UPROPERTY(EditAnywhere, Category = "Battle Area Teleport|Config", meta = (DisplayName = "退出战斗延迟"))
	float ExitBattleDelay = 0.5f;

	UPROPERTY(EditAnywhere, Category = "Battle Area Teleport|Config", meta = (DisplayName = "启用平滑相机过渡"))
	bool bUseSmoothCameraTransition = true;

	UPROPERTY(EditAnywhere, Category = "Battle Area Teleport|Config", meta = (DisplayName = "相机过渡持续时间"))
	float CameraTransitionDuration = 1.0f;

	// 战斗前保存的玩家原始位置和旋转
	FVector SavedPlayerLocation = FVector::ZeroVector;
	FRotator SavedPlayerRotation = FRotator::ZeroRotator;
	bool bHasSavedPlayerTransform = false;

	// 战斗前保存的敌人原始位置和旋转
	FVector SavedEnemyLocation = FVector::ZeroVector;
	FRotator SavedEnemyRotation = FRotator::ZeroRotator;
	bool bHasSavedEnemyTransform = false;

	// 当前战斗的敌人引用（用于退出时恢复位置）
	TWeakObjectPtr<ABaseEnemy> CachedBattleEnemy;
};