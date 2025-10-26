// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BattleAreaManager.generated.h"

class ABaseEnemy;
class AAreaCollisionBox;

/**
 * 战斗区域管理器
 * 负责管理敌人与区域的关联关系
 */
UCLASS(BlueprintType)
class CURRSOR_API UBattleAreaManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	/**
	 * 设置敌人的区域ID
	 * @param Enemy 敌人角色
	 * @param AreaID 区域ID
	 */
	UFUNCTION(BlueprintCallable, Category = "Battle Area Manager")
	void SetEnemyAreaID(ABaseEnemy* Enemy, int32 AreaID);

	/**
	 * 获取敌人的区域ID
	 * @param Enemy 敌人角色
	 * @return 区域ID，如果未设置返回-1
	 */
	UFUNCTION(BlueprintPure, Category = "Battle Area Manager")
	int32 GetEnemyAreaID(ABaseEnemy* Enemy) const;

	/**
	 * 批量设置区域内的敌人
	 * @param AreaBox 区域碰撞盒
	 * @param SearchRadius 搜索半径
	 */
	UFUNCTION(BlueprintCallable, Category = "Battle Area Manager")
	void AssignEnemiesInArea(AAreaCollisionBox* AreaBox, float SearchRadius = 500.0f);

	/**
	 * 清除敌人的区域ID
	 * @param Enemy 敌人角色
	 */
	UFUNCTION(BlueprintCallable, Category = "Battle Area Manager")
	void ClearEnemyAreaID(ABaseEnemy* Enemy);

	/**
	 * 获取指定区域的所有敌人
	 * @param AreaID 区域ID
	 * @return 敌人数组
	 */
	UFUNCTION(BlueprintPure, Category = "Battle Area Manager")
	TArray<ABaseEnemy*> GetEnemiesInArea(int32 AreaID) const;

	/**
	 * 检查敌人是否有区域ID
	 * @param Enemy 敌人角色
	 * @return 是否有区域ID
	 */
	UFUNCTION(BlueprintPure, Category = "Battle Area Manager")
	bool DoesEnemyHaveAreaID(ABaseEnemy* Enemy) const;

protected:
	/**
	 * 查找区域内的敌人
	 * @param CenterLocation 中心位置
	 * @param SearchRadius 搜索半径
	 * @return 找到的敌人数组
	 */
	UFUNCTION(BlueprintPure, Category = "Battle Area Manager")
	TArray<ABaseEnemy*> FindEnemiesInRadius(const FVector& CenterLocation, float SearchRadius) const;

private:
	// 敌人与区域ID的映射
	UPROPERTY()
	TMap<TWeakObjectPtr<ABaseEnemy>, int32> EnemyAreaMap;

	// 区域ID与敌人的映射
	TMap<int32, TArray<TWeakObjectPtr<ABaseEnemy>>> AreaEnemyMap;

	// 清理无效的弱引用
	void CleanupInvalidReferences();
};