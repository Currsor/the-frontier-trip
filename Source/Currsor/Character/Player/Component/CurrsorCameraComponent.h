// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "CurrsorCameraComponent.generated.h"

/**
 * 
 */
UCLASS()
class CURRSOR_API UCurrsorCameraComponent : public UCameraComponent
{
	GENERATED_BODY()

	UCurrsorCameraComponent();

public:
	/**
	 * 更新景深效果
	 * @param FocalDistance 焦距
	 * @param bIsCollision 是否处于碰撞状态
	 * @param TargetArmLength 目标臂长（弹簧臂的原始长度）
	 */
	UFUNCTION()
	void UpdateDOF(float FocalDistance, bool bIsCollision = false, float TargetArmLength = 0.0f);

private:
	// 是否处于碰撞状态
	UPROPERTY()
	bool bWasInCollision;
};
