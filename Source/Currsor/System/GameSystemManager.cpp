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
    UE_LOG(LogTemp, Log, TEXT("GameSystemManager created"));
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
        UE_LOG(LogTemp, Error, TEXT("GameSystemManager::GetInstance - No valid world found"));
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
    
    UE_LOG(LogTemp, Log, TEXT("GameSystemManager instance created for world: %s"), 
           InWorld ? *InWorld->GetName() : TEXT("Unknown"));
    
    return NewInstance;
}

void UGameSystemManager::Initialize(UWorld* InWorld)
{
    if (bIsInitialized)
    {
        UE_LOG(LogTemp, Warning, TEXT("GameSystemManager already initialized"));
        return;
    }

    if (!InWorld)
    {
        UE_LOG(LogTemp, Error, TEXT("GameSystemManager::Initialize - Invalid world"));
        return;
    }

    World = InWorld;
    UE_LOG(LogTemp, Log, TEXT("Initializing GameSystemManager..."));

    try
    {
        // 按依赖顺序初始化系统
        InitializeCore();
        InitializeManagers();
        InitializeSystems();
        SetupSystemConnections();

        bIsInitialized = true;
        UE_LOG(LogTemp, Log, TEXT("GameSystemManager initialized successfully"));

        // 广播初始化完成事件
        OnGameSystemsInitialized.Broadcast(FPlatformTime::Seconds());
    }
    catch (...)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to initialize GameSystemManager"));
        bIsInitialized = false;
    }
}

void UGameSystemManager::InitializeCore()
{
    UE_LOG(LogTemp, Log, TEXT("Initializing core systems..."));
    
    // 核心系统初始化（配置、事件等）
    // 这些通常是静态的或全局的
    
    UE_LOG(LogTemp, Log, TEXT("Core systems initialized"));
}

void UGameSystemManager::InitializeManagers()
{
    UE_LOG(LogTemp, Log, TEXT("Initializing managers..."));
    
    if (!World.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("Invalid world reference during manager initialization"));
        return;
    }

    // 游戏逻辑管理器
    GameLogicManager = NewObject<UGameLogicManagerComponent>(this);
    if (GameLogicManager)
    {
        GameLogicManager->Initialize();
        UE_LOG(LogTemp, Log, TEXT("GameLogicManager initialized"));
    }

    // 状态管理器
    StateManager = NewObject<UStateManagerComponent>(this);
    if (StateManager)
    {
        StateManager->Initialize();
        UE_LOG(LogTemp, Log, TEXT("StateManager initialized"));
    }
    
    UE_LOG(LogTemp, Log, TEXT("Managers initialized"));
}

void UGameSystemManager::InitializeSystems()
{
    UE_LOG(LogTemp, Log, TEXT("Initializing systems..."));
    
    // 攻击系统
    AttackSystem = NewObject<UAttackSystemComponent>(this);
    if (AttackSystem)
    {
        AttackSystem->Initialize();
        UE_LOG(LogTemp, Log, TEXT("AttackSystem initialized"));
    }

    // 掉落系统
    LootSystem = NewObject<ULootSystemComponent>(this);
    if (LootSystem)
    {
        LootSystem->Initialize();
        UE_LOG(LogTemp, Log, TEXT("LootSystem initialized"));
    }
    
    UE_LOG(LogTemp, Log, TEXT("Systems initialized"));
}

void UGameSystemManager::SetupSystemConnections()
{
    UE_LOG(LogTemp, Log, TEXT("Setting up system connections..."));
    
    // 设置系统间的连接
    SetupAttackSystemConnections();
    SetupLootSystemConnections();
    SetupUIConnections();
    
    UE_LOG(LogTemp, Log, TEXT("System connections established"));
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
    UE_LOG(LogTemp, Log, TEXT("Resetting all systems..."));

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

    UE_LOG(LogTemp, Log, TEXT("All systems reset"));
}

void UGameSystemManager::Shutdown()
{
    UE_LOG(LogTemp, Log, TEXT("Shutting down GameSystemManager..."));

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

    UE_LOG(LogTemp, Log, TEXT("GameSystemManager shutdown complete"));
}

void UGameSystemManager::DebugPrintStatus() const
{
    UE_LOG(LogTemp, Log, TEXT("=== Game System Manager Status ==="));
    UE_LOG(LogTemp, Log, TEXT("Initialized: %s"), bIsInitialized ? TEXT("True") : TEXT("False"));
    UE_LOG(LogTemp, Log, TEXT("World: %s"), World.IsValid() ? *World->GetName() : TEXT("Invalid"));
    
    UE_LOG(LogTemp, Log, TEXT("Systems:"));
    UE_LOG(LogTemp, Log, TEXT("- AttackSystem: %s"), AttackSystem ? TEXT("Active") : TEXT("Inactive"));
    UE_LOG(LogTemp, Log, TEXT("- StateManager: %s"), StateManager ? TEXT("Active") : TEXT("Inactive"));
    UE_LOG(LogTemp, Log, TEXT("- LootSystem: %s"), LootSystem ? TEXT("Active") : TEXT("Inactive"));
    UE_LOG(LogTemp, Log, TEXT("- GameLogicManager: %s"), GameLogicManager ? TEXT("Active") : TEXT("Inactive"));
    
    UE_LOG(LogTemp, Log, TEXT("=================================="));
}