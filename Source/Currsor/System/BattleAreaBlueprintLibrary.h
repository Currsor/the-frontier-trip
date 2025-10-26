// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "BattleAreaBlueprintLibrary.generated.h"

class ABaseEnemy;
class AAreaCollisionBox;
class UBattleAreaManager;

/**
 * 战斗区域蓝图函数库
 * 提供便于在蓝图中使用的战斗区域相关功能
 */
UCLASS()
class CURRSOR_API UBattleAreaBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	 * 获取战斗区域管理器
	 * @param WorldContext 世界上下文
	 * @return 战斗区域管理器
	 */
	UFUNCTION(BlueprintPure, Category = "Battle Area", CallInEditor, meta = (WorldContext = "WorldContext"))
	static UBattleAreaManager* GetBattleAreaManager(const UObject* WorldContext);

	/**
	 * 设置敌人的区域ID
	 * @param Enemy 敌人角色
	 * @param AreaID 区域ID
	 */
	UFUNCTION(BlueprintCallable, Category = "Battle Area", CallInEditor)
	static void SetEnemyAreaID(ABaseEnemy* Enemy, int32 AreaID);

	/**
	 * 获取敌人的区域ID
	 * @param Enemy 敌人角色
	 * @return 区域ID
	 */
	UFUNCTION(BlueprintPure, Category = "Battle Area")
	static int32 GetEnemyAreaID(ABaseEnemy* Enemy);

	/**
	 * 批量设置区域内的敌人
	 * @param AreaBox 区域碰撞盒
	 * @param SearchRadius 搜索半径
	 */
	UFUNCTION(BlueprintCallable, Category = "Battle Area", CallInEditor)
	static void AssignEnemiesInArea(AAreaCollisionBox* AreaBox, float SearchRadius = 500.0f);

	/**
	 * 检查敌人是否有区域ID
	 * @param Enemy 敌人角色
	 * @return 是否有区域ID
	 */
	UFUNCTION(BlueprintPure, Category = "Battle Area")
	static bool DoesEnemyHaveAreaID(ABaseEnemy* Enemy);

	/**
	 * 获取指定区域的所有敌人
	 * @param WorldContext 世界上下文
	 * @param AreaID 区域ID
	 * @return 敌人数组
	 */
	UFUNCTION(BlueprintPure, Category = "Battle Area", meta = (WorldContext = "WorldContext"))
	static TArray<ABaseEnemy*> GetEnemiesInArea(const UObject* WorldContext, int32 AreaID);

	/**
	 * 清除敌人的区域ID
	 * @param Enemy 敌人角色
	 */
	UFUNCTION(BlueprintCallable, Category = "Battle Area", CallInEditor)
	static void ClearEnemyAreaID(ABaseEnemy* Enemy);

	/**
	 * 获取区域碰撞盒的位置信息
	 * @param AreaBox 区域碰撞盒
	 * @param PlayerPosition 玩家位置
	 * @param EnemyPosition 敌人位置
	 * @param CameraPosition 相机位置
	 * @return 是否成功获取位置信息
	 */
	UFUNCTION(BlueprintPure, Category = "Battle Area")
	static bool GetAreaPositions(AAreaCollisionBox* AreaBox, FVector& PlayerPosition, FVector& EnemyPosition, FVector& CameraPosition);

	/**
	 * 验证战斗区域传送条件
	 * @param Player 玩家角色
	 * @param Enemy 敌人角色
	 * @return 是否满足传送条件
	 */
	UFUNCTION(BlueprintPure, Category = "Battle Area")
	static bool ValidateBattleTeleportConditions(class ACurrsorCharacter* Player, ABaseEnemy* Enemy);

	// 玩家区域ID相关函数
	/**
	 * 设置玩家的区域ID
	 * @param Player 玩家角色
	 * @param AreaID 区域ID
	 */
	UFUNCTION(BlueprintCallable, Category = "Battle Area|Player", CallInEditor)
	static void SetPlayerAreaID(class ACurrsorCharacter* Player, int32 AreaID);

	/**
	 * 获取玩家的区域ID
	 * @param Player 玩家角色
	 * @return 区域ID
	 */
	UFUNCTION(BlueprintPure, Category = "Battle Area|Player")
	static int32 GetPlayerAreaID(class ACurrsorCharacter* Player);

	/**
	 * 检查玩家是否有有效的区域ID
	 * @param Player 玩家角色
	 * @return 是否有有效的区域ID
	 */
	UFUNCTION(BlueprintPure, Category = "Battle Area|Player")
	static bool DoesPlayerHaveValidAreaID(class ACurrsorCharacter* Player);

	/**
	 * 清除玩家的区域ID
	 * @param Player 玩家角色
	 */
	UFUNCTION(BlueprintCallable, Category = "Battle Area|Player", CallInEditor)
	static void ClearPlayerAreaID(class ACurrsorCharacter* Player);

	/**
	 * 获取将要使用的区域ID（优先玩家，其次敌人）
	 * @param Player 玩家角色
	 * @param Enemy 敌人角色
	 * @param OutAreaID 输出的区域ID
	 * @param OutIsFromPlayer 是否来自玩家
	 * @return 是否找到有效的区域ID
	 */
	UFUNCTION(BlueprintPure, Category = "Battle Area")
	static bool GetEffectiveAreaID(class ACurrsorCharacter* Player, ABaseEnemy* Enemy, int32& OutAreaID, bool& OutIsFromPlayer);

	// AreaCollisionBox选择相关函数
	/**
	 * 设置敌人选中的区域碰撞盒
	 * @param Enemy 敌人角色
	 * @param AreaBox 区域碰撞盒
	 */
	UFUNCTION(BlueprintCallable, Category = "Battle Area|AreaBox", CallInEditor)
	static void SetEnemySelectedAreaBox(ABaseEnemy* Enemy, AAreaCollisionBox* AreaBox);

	/**
	 * 获取敌人选中的区域碰撞盒
	 * @param Enemy 敌人角色
	 * @return 选中的区域碰撞盒
	 */
	UFUNCTION(BlueprintPure, Category = "Battle Area|AreaBox")
	static AAreaCollisionBox* GetEnemySelectedAreaBox(ABaseEnemy* Enemy);

	/**
	 * 从选中的区域碰撞盒读取AreaID
	 * @param Enemy 敌人角色
	 */
	UFUNCTION(BlueprintCallable, Category = "Battle Area|AreaBox", CallInEditor)
	static void ReadEnemyAreaIDFromSelectedBox(ABaseEnemy* Enemy);

	/**
	 * 检查敌人是否有选中的区域碰撞盒
	 * @param Enemy 敌人角色
	 * @return 是否有选中的区域碰撞盒
	 */
	UFUNCTION(BlueprintPure, Category = "Battle Area|AreaBox")
	static bool DoesEnemyHaveSelectedAreaBox(ABaseEnemy* Enemy);

	// ========== 测试假人管理 ==========
	/**
	 * 为指定区域生成测试假人
	 * @param AreaBox 区域碰撞盒
	 */
	UFUNCTION(BlueprintCallable, Category = "Battle Area|Test Dummies", CallInEditor)
	static void SpawnTestDummiesForArea(AAreaCollisionBox* AreaBox);

	/**
	 * 销毁指定区域的测试假人
	 * @param AreaBox 区域碰撞盒
	 */
	UFUNCTION(BlueprintCallable, Category = "Battle Area|Test Dummies", CallInEditor)
	static void DestroyTestDummiesForArea(AAreaCollisionBox* AreaBox);

	/**
	 * 为所有区域生成测试假人
	 * @param WorldContext 世界上下文
	 */
	UFUNCTION(BlueprintCallable, Category = "Battle Area|Test Dummies", CallInEditor, meta = (WorldContext = "WorldContext"))
	static void SpawnAllTestDummies(const UObject* WorldContext);

	/**
	 * 销毁所有区域的测试假人
	 * @param WorldContext 世界上下文
	 */
	UFUNCTION(BlueprintCallable, Category = "Battle Area|Test Dummies", CallInEditor, meta = (WorldContext = "WorldContext"))
	static void DestroyAllTestDummies(const UObject* WorldContext);
};