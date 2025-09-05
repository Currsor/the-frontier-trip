// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "CurrsorGameMode.generated.h"

// 声明一个动态多播委托，用于在游戏模式中广播战斗状态变化事件
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCombatStateChangedDelegate, const FString&, CombatEventType);

/**
 * 游戏模式类，管理游戏的各种状态
 */
UCLASS()
class CURRSOR_API ACurrsorGameMode : public AGameMode
{
	GENERATED_BODY()
public:
	virtual void StartPlay() override;

	UPROPERTY(BlueprintAssignable, Category = "Combat")
	FOnCombatStateChangedDelegate OnCombatStateChanged;

	UFUNCTION()
	void HandleCombatStarted(const FString& CombatEventType);
};
