// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "CurrsorGameMode.generated.h"

/**
 * 游戏模式类，管理游戏的各种状态
 */
UCLASS()
class CURRSOR_API ACurrsorGameMode : public AGameMode
{
	GENERATED_BODY()
public:
	virtual void StartPlay() override;
};
