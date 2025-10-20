// Fill out your copyright notice in the Description page of Project Settings.

#include "GameSystemManager.h"
#include "Engine/World.h"
#include "Components/AttackSystemComponent.h"
#include "Components/StateManagerComponent.h"
#include "Components/LootSystemComponent.h"
#include "Components/GameLogicManagerComponent.h"

// 静态成员初始化
TMap<TWeakObjectPtr<UWorld>, TWeakObjectPtr<UGameSystemManager>> UGameSystemManager::Instances;

UGameSystemManager::UGameSystemManager()
{
    UE_LOG(LogTemp, Log, TEXT("GameSystemManager 已创建"));
}

UGameSystemManager* UGameSystemManager::GetInstance(UWorld* InWorld)
{
    if (!InWorld)
    {
        if (GEngine && GEngine->GetWorldContexts().Num() > 0)
        {
            InWorld = GEngine->GetWorldContexts()[0].World();
        }
    }

    if (!InWorld)
    {
        UE_LOG(LogTemp, Error, TEXT("GameSystemManager::GetInstance - 未找到有效的世界"));
        return nullptr;
    }

    // 清理无效的弱引用
    for (auto It = Instances.CreateIterator(); It; ++It)
    {
        if (!It.Key().IsValid() || !It.Value().IsValid())
        {
            It.RemoveCurrent();
        }
    }

    // 查找现有实例
    TWeakObjectPtr<UWorld> WorldPtr(InWorld);
    if (auto* ExistingInstance = Instances.Find(WorldPtr))
    {
        if (ExistingInstance->IsValid())
        {
            return ExistingInstance->Get();
        }
        else
        {
            Instances.Remove(WorldPtr);
        }
    }

    // 创建新实例
    UGameSystemManager* NewInstance = NewObject<UGameSystemManager>(InWorld);
    Instances.Add(WorldPtr, NewInstance);
    
    UE_LOG(LogTemp, Log, TEXT("为世界 %s 创建了 GameSystemManager 实例"), 
           InWorld ? *InWorld->GetName() : TEXT("未知"));
    
    return NewInstance;
}

void UGameSystemManager::Initialize(UWorld* InWorld)
{
    if (bIsInitialized)
    {
        UE_LOG(LogTemp, Warning, TEXT("GameSystemManager 已初始化"));
        return;
    }

    if (!InWorld)
    {
        UE_LOG(LogTemp, Error, TEXT("GameSystemManager::Initialize - 世界无效"));
        return;
    }

    World = InWorld;
    UE_LOG(LogTemp, Log, TEXT("正在初始化 GameSystemManager..."));

    // 按依赖顺序初始化系统
    bool bInitSuccess = true;
    
    if (bInitSuccess)
    {
        bInitSuccess = InitializeCore();
    }
    
    if (bInitSuccess)
    {
        bInitSuccess = InitializeManagers();
    }
    
    if (bInitSuccess)
    {
        bInitSuccess = InitializeSystems();
    }
    
    if (bInitSuccess)
    {
        bInitSuccess = SetupSystemConnections();
    }

    if (bInitSuccess)
    {
        bIsInitialized = true;
        UE_LOG(LogTemp, Log, TEXT("GameSystemManager 初始化成功"));

        // 广播初始化完成事件
        OnGameSystemsInitialized.Broadcast(FPlatformTime::Seconds());
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("初始化 GameSystemManager 失败"));
        bIsInitialized = false;
    }
}

bool UGameSystemManager::InitializeCore()
{
    UE_LOG(LogTemp, Log, TEXT("正在初始化核心系统..."));
    
    // 核心系统初始化（配置、事件等）
    // 这些通常是静态的或全局的
    
    UE_LOG(LogTemp, Log, TEXT("核心系统已初始化"));
    return true;
}

bool UGameSystemManager::InitializeManagers()
{
    UE_LOG(LogTemp, Log, TEXT("正在初始化管理器..."));
    
    if (!World.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("管理器初始化期间世界引用无效"));
        return false;
    }

    // 游戏逻辑管理器
    GameLogicManager = NewObject<UGameLogicManagerComponent>(this);
    if (!GameLogicManager)
    {
        UE_LOG(LogTemp, Error, TEXT("创建 GameLogicManager 失败"));
        return false;
    }
    GameLogicManager->Initialize();
    UE_LOG(LogTemp, Log, TEXT("GameLogicManager 已初始化"));

    // 状态管理器
    StateManager = NewObject<UStateManagerComponent>(this);
    if (!StateManager)
    {
        UE_LOG(LogTemp, Error, TEXT("创建 StateManager 失败"));
        return false;
    }
    StateManager->Initialize();
    UE_LOG(LogTemp, Log, TEXT("StateManager 已初始化"));
    
    UE_LOG(LogTemp, Log, TEXT("管理器已初始化"));
    return true;
}

bool UGameSystemManager::InitializeSystems()
{
    UE_LOG(LogTemp, Log, TEXT("正在初始化系统..."));
    
    // 攻击系统
    AttackSystem = NewObject<UAttackSystemComponent>(this);
    if (!AttackSystem)
    {
        UE_LOG(LogTemp, Error, TEXT("创建 AttackSystem 失败"));
        return false;
    }
    AttackSystem->Initialize();
    UE_LOG(LogTemp, Log, TEXT("AttackSystem 已初始化"));

    // 掉落系统
    LootSystem = NewObject<ULootSystemComponent>(this);
    if (!LootSystem)
    {
        UE_LOG(LogTemp, Error, TEXT("创建 LootSystem 失败"));
        return false;
    }
    LootSystem->Initialize();
    UE_LOG(LogTemp, Log, TEXT("LootSystem 已初始化"));
    
    UE_LOG(LogTemp, Log, TEXT("系统已初始化"));
    return true;
}

bool UGameSystemManager::SetupSystemConnections()
{
    UE_LOG(LogTemp, Log, TEXT("正在设置系统连接..."));
    
    // 设置系统间的连接
    SetupAttackSystemConnections();
    SetupLootSystemConnections();
    SetupUIConnections();
    
    UE_LOG(LogTemp, Log, TEXT("系统连接已建立"));
    return true;
}

void UGameSystemManager::SetupAttackSystemConnections()
{
    // 攻击系统连接将在各个组件内部处理
    // 这里可以设置跨系统的连接
}

void UGameSystemManager::SetupLootSystemConnections()
{
    // 掉落系统连接
}

void UGameSystemManager::SetupUIConnections()
{
    // UI连接
}

void UGameSystemManager::ResetAllSystems()
{
    UE_LOG(LogTemp, Log, TEXT("正在重置所有系统..."));

    if (AttackSystem)
    {
        AttackSystem->Reset();
    }

    if (StateManager)
    {
        StateManager->Reset();
    }

    if (LootSystem)
    {
        LootSystem->Reset();
    }

    if (GameLogicManager)
    {
        GameLogicManager->Reset();
    }

    UE_LOG(LogTemp, Log, TEXT("所有系统已重置"));
}

void UGameSystemManager::Shutdown()
{
    UE_LOG(LogTemp, Log, TEXT("正在关闭 GameSystemManager..."));

    // 清理系统
    if (AttackSystem)
    {
        AttackSystem->Shutdown();
        AttackSystem = nullptr;
    }

    if (StateManager)
    {
        StateManager->Shutdown();
        StateManager = nullptr;
    }

    if (LootSystem)
    {
        LootSystem->Shutdown();
        LootSystem = nullptr;
    }

    if (GameLogicManager)
    {
        GameLogicManager->Shutdown();
        GameLogicManager = nullptr;
    }

    bIsInitialized = false;
    World.Reset();

    UE_LOG(LogTemp, Log, TEXT("GameSystemManager 关闭完成"));
}

void UGameSystemManager::DebugPrintStatus() const
{
    UE_LOG(LogTemp, Log, TEXT("=== 游戏系统管理器状态 ==="));
    UE_LOG(LogTemp, Log, TEXT("已初始化: %s"), bIsInitialized ? TEXT("是") : TEXT("否"));
    UE_LOG(LogTemp, Log, TEXT("世界: %s"), World.IsValid() ? *World->GetName() : TEXT("无效"));
    
    UE_LOG(LogTemp, Log, TEXT("系统:"));
    UE_LOG(LogTemp, Log, TEXT("- 攻击系统: %s"), AttackSystem ? TEXT("活跃") : TEXT("非活跃"));
    UE_LOG(LogTemp, Log, TEXT("- 状态管理器: %s"), StateManager ? TEXT("活跃") : TEXT("非活跃"));
    UE_LOG(LogTemp, Log, TEXT("- 战利品系统: %s"), LootSystem ? TEXT("活跃") : TEXT("非活跃"));
    UE_LOG(LogTemp, Log, TEXT("- 游戏逻辑管理器: %s"), GameLogicManager ? TEXT("活跃") : TEXT("非活跃"));
    
    UE_LOG(LogTemp, Log, TEXT("=================================="));
}