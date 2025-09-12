// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/World.h"
#include "GameSystemManager.generated.h"

class UAttackSystemComponent;
class UStateManagerComponent;
class ULootSystemComponent;
class UGameLogicManagerComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGameSystemsInitialized, float, Timestamp);

/**
 * 游戏系统管理器
 * 统一管理所有游戏系统的初始化、更新和销毁
 * 对应TypeScript中的GameSystemManager
 */
UCLASS(BlueprintType, Blueprintable)
class CURRSOR_API UGameSystemManager : public UObject
{
    GENERATED_BODY()

public:
    UGameSystemManager();

    // 单例访问
    UFUNCTION(BlueprintCallable, Category = "Game System Manager", CallInEditor)
    static UGameSystemManager* GetInstance(UWorld* World = nullptr);

    // 系统生命周期管理
    UFUNCTION(BlueprintCallable, Category = "Game System Manager")
    void Initialize(UWorld* InWorld);

    UFUNCTION(BlueprintCallable, Category = "Game System Manager")
    void Shutdown();

    UFUNCTION(BlueprintCallable, Category = "Game System Manager")
    void ResetAllSystems();

    // 系统访问
    UFUNCTION(BlueprintCallable, Category = "Game System Manager")
    UAttackSystemComponent* GetAttackSystem() const { return AttackSystem; }

    UFUNCTION(BlueprintCallable, Category = "Game System Manager")
    UStateManagerComponent* GetStateManager() const { return StateManager; }

    UFUNCTION(BlueprintCallable, Category = "Game System Manager")
    ULootSystemComponent* GetLootSystem() const { return LootSystem; }

    UFUNCTION(BlueprintCallable, Category = "Game System Manager")
    UGameLogicManagerComponent* GetGameLogicManager() const { return GameLogicManager; }

    // 状态查询
    UFUNCTION(BlueprintPure, Category = "Game System Manager")
    bool IsInitialized() const { return bIsInitialized; }

    UFUNCTION(BlueprintCallable, Category = "Game System Manager")
    void DebugPrintStatus() const;

    // 事件
    UPROPERTY(BlueprintAssignable, Category = "Game System Manager")
    FOnGameSystemsInitialized OnGameSystemsInitialized;

protected:
    // 初始化步骤
    void InitializeCore();
    void InitializeManagers();
    void InitializeSystems();
    void SetupSystemConnections();

    // 系统连接设置
    void SetupAttackSystemConnections();
    void SetupLootSystemConnections();
    void SetupUIConnections();

private:
    // 单例实例
    static TMap<TWeakObjectPtr<UWorld>, TWeakObjectPtr<UGameSystemManager>> Instances;

    // 初始化状态
    UPROPERTY(VisibleAnywhere, Category = "Game System Manager")
    bool bIsInitialized = false;

    // 世界引用
    UPROPERTY()
    TWeakObjectPtr<UWorld> World;

    // 系统组件
    UPROPERTY(VisibleAnywhere, Category = "Systems")
    TObjectPtr<UAttackSystemComponent> AttackSystem;

    UPROPERTY(VisibleAnywhere, Category = "Systems")
    TObjectPtr<UStateManagerComponent> StateManager;

    UPROPERTY(VisibleAnywhere, Category = "Systems")
    TObjectPtr<ULootSystemComponent> LootSystem;

    UPROPERTY(VisibleAnywhere, Category = "Systems")
    TObjectPtr<UGameLogicManagerComponent> GameLogicManager;
};