// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseSystemComponent.h"
#include "GameLogicManagerComponent.generated.h"

/**
 * 游戏逻辑管理器组件
 * 处理核心游戏逻辑和业务规则
 * 对应TypeScript中的GameLogicManager
 */
UCLASS(BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CURRSOR_API UGameLogicManagerComponent : public UBaseSystemComponent
{
    GENERATED_BODY()

public:
    UGameLogicManagerComponent();

    // 游戏逻辑处理
    UFUNCTION(BlueprintCallable, Category = "Game Logic")
    bool ProcessGameEvent(const FString& EventType, const TMap<FString, FString>& EventData);

    UFUNCTION(BlueprintCallable, Category = "Game Logic")
    void UpdateGameState(float DeltaTime);

protected:
    virtual void OnInitialize() override;
    virtual void OnReset() override;
    virtual void OnShutdown() override;

private:
    UPROPERTY(EditAnywhere, Category = "Game Logic")
    bool bEnableDebugLogging = true;
};