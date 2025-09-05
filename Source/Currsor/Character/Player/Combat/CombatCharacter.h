// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperZDCharacter.h"
#include "Currsor/Character/Player/Component/CurrsorCameraComponent.h"
#include "CombatCharacter.generated.h"

/**
 * 战斗玩家角色类
 */
UCLASS()
class CURRSOR_API ACombatCharacter : public APaperZDCharacter
{
	GENERATED_BODY()

public:
	ACombatCharacter();

private:
	// 相机组件
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCurrsorCameraComponent> CameraComponent;
};
