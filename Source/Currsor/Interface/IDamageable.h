// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IDamageable.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UDamageable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class CURRSOR_API IDamageable
{
	GENERATED_BODY()

public:
	/** 对实现此接口的对象施加伤害 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Damage")
	void ApplyDamage(
		float DamageAmount,          // 伤害值
		AActor* DamageInstigator,    // 造成伤害的发起者（例如玩家角色、发射子弹的敌人）
		const FHitResult& HitResult  // 可选的命中结果，包含碰撞位置、法线等详细信息
	);
public:
};
