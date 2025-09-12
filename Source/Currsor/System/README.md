# C++ 架构更新文档

## 📋 概述

本次更新将TypeScript架构模式移植到C++代码中，实现了分层架构设计，提高了代码的可维护性和扩展性。

## 🏗️ 架构层次

```
┌─────────────────────────────────────────────────────────────┐
│                    TypeScript Layer                        │
│  ├── GameSystemManager (业务逻辑)                           │
│  ├── EventSystem (事件通信)                                 │
│  ├── StateManager (状态管理)                                │
│  └── AttackSystem (攻击逻辑)                                │
├─────────────────────────────────────────────────────────────┤
│                    C++ System Layer                        │
│  ├── GameSystemManager (系统管理器)                         │
│  ├── AttackSystemComponent (攻击系统)                       │
│  ├── StateManagerComponent (状态管理)                       │
│  ├── LootSystemComponent (掉落系统)                         │
│  └── GameLogicManagerComponent (游戏逻辑)                   │
├─────────────────────────────────────────────────────────────┤
│                    C++ Core Layer                          │
│  ├── BaseState (状态基类)                                   │
│  ├── HealthComponent (生命值组件)                            │
│  ├── DestructibleItem (可破坏物品)                          │
│  └── CurrsorCharacter (角色核心)                            │
└─────────────────────────────────────────────────────────────┘
```

## 📁 新增文件结构

```
Source/Currsor/System/
├── GameSystemManager.h/.cpp          # 系统管理器入口
└── Components/
    ├── BaseSystemComponent.h/.cpp     # 系统组件基类
    ├── AttackSystemComponent.h/.cpp   # 攻击系统
    ├── StateManagerComponent.h/.cpp   # 状态管理
    ├── LootSystemComponent.h/.cpp     # 掉落系统
    └── GameLogicManagerComponent.h/.cpp # 游戏逻辑管理
```

## 🎯 核心组件介绍

### 1. GameSystemManager
- **职责**: 统一管理所有游戏系统的生命周期
- **功能**: 初始化、更新、重置、销毁所有系统
- **使用**: 单例模式，通过 `UGameSystemManager::GetInstance()` 获取

### 2. BaseSystemComponent
- **职责**: 为所有系统组件提供统一的基类和接口
- **功能**: 标准化的初始化、重置、销毁流程
- **特点**: 抽象基类，所有系统组件都继承自它

### 3. AttackSystemComponent
- **职责**: 处理攻击逻辑、伤害计算和攻击事件
- **功能**: 攻击验证、伤害计算、冷却管理、事件广播
- **事件**: OnAttackHit, OnAttackStarted, OnAttackEnd

### 4. StateManagerComponent
- **职责**: 管理角色状态转换和验证
- **功能**: 状态优先级、转换规则、状态生命周期管理
- **特点**: 支持自定义转换规则和状态优先级

### 5. LootSystemComponent
- **职责**: 处理物品掉落逻辑和掉落表管理
- **功能**: 掉落概率计算、掉落表管理、掉落历史记录
- **配置**: 支持全局掉落率倍数和自定义掉落表

## 🔄 集成方式

### 在PlayerController中集成
```cpp
// 头文件声明
UPROPERTY(BlueprintReadOnly, Category = "Systems")
TObjectPtr<UGameSystemManager> GameSystemManager;

// BeginPlay中初始化
GameSystemManager = UGameSystemManager::GetInstance(GetWorld());
if (GameSystemManager && !GameSystemManager->IsInitialized())
{
    GameSystemManager->Initialize(GetWorld());
}
```

### 在Character中集成
```cpp
// 获取系统引用
if (GameSystemManager)
{
    AttackSystem = GameSystemManager->GetAttackSystem();
    StateManager = GameSystemManager->GetStateManager();
}
```

## 📊 系统特性

### 1. 生命周期管理
- **Initialize()**: 系统初始化
- **Reset()**: 系统重置
- **Shutdown()**: 系统销毁

### 2. 事件驱动架构
- 系统间通过事件进行通信
- 支持事件优先级和历史记录
- 松耦合设计

### 3. 配置化管理
- 支持运行时配置修改
- 全局参数统一管理
- 调试信息输出

### 4. 统计和调试
- 系统状态监控
- 性能统计数据
- 详细的调试日志

## 🚀 使用示例

### 攻击系统使用
```cpp
// 开始攻击
if (AttackSystem && AttackSystem->CanAttack(Player))
{
    AttackSystem->StartAttack(Player, TEXT("Normal"));
}

// 处理攻击
FAttackData AttackData;
AttackData.BaseDamage = 25.0f;
AttackData.CriticalChance = 0.15f;
AttackSystem->ProcessAttack(Attacker, Target, AttackData);
```

### 状态管理使用
```cpp
// 改变状态
StateManager->ChangeState(Player, ECharacterState::Attack);

// 检查状态
if (StateManager->IsInState(Player, ECharacterState::Idle))
{
    // 执行空闲逻辑
}
```

### 掉落系统使用
```cpp
// 生成掉落
TArray<FLootItem> Loot = LootSystem->GenerateLoot(Enemy, TEXT("BossLoot"));

// 生成掉落物品
LootSystem->SpawnLoot(Enemy, Loot, Enemy->GetActorLocation());
```

## 🔧 配置选项

### 攻击系统配置
- `GlobalDamageMultiplier`: 全局伤害倍数
- `AttackCooldown`: 攻击冷却时间
- `bEnableDebugLogging`: 启用调试日志

### 状态管理配置
- `StatePriorities`: 状态优先级映射
- `TransitionRules`: 状态转换规则
- `DefaultMinStateDuration`: 默认最小状态持续时间

### 掉落系统配置
- `GlobalDropRateMultiplier`: 全局掉落率倍数
- `LootTables`: 掉落表配置
- `RecentDropHistory`: 掉落历史记录

## 🎯 优势总结

1. **职责分离**: 每个系统专注于特定功能
2. **统一管理**: 通过GameSystemManager统一管理所有系统
3. **事件驱动**: 系统间松耦合通信
4. **可扩展性**: 易于添加新的系统组件
5. **调试友好**: 丰富的调试信息和状态监控
6. **配置灵活**: 支持运行时配置修改
7. **性能优化**: 按需初始化和更新

## 📝 注意事项

1. 确保在使用系统前先初始化GameSystemManager
2. 系统组件的生命周期由GameSystemManager管理
3. 使用事件系统时注意避免循环依赖
4. 调试模式下会输出详细日志，发布版本建议关闭
5. 系统重置会清理所有运行时数据，谨慎使用

## 🔄 与TypeScript的协作

C++系统层专注于：
- 性能敏感的核心逻辑
- 底层系统管理
- 数据结构和算法

TypeScript层专注于：
- 业务逻辑和规则
- UI交互和事件处理
- 快速迭代的功能

两层通过事件系统和接口进行通信，实现了职责分离和最佳性能。