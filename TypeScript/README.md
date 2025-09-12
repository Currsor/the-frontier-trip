# TypeScript 游戏架构系统

本项目采用了分层的TypeScript架构，将游戏逻辑从C++层分离到TypeScript层，提供更好的可维护性和开发效率。

## 🏗️ 架构概览

```
┌─────────────────────────────────────────────────────────────┐
│                    TypeScript Logic Layer                   │
├─────────────────────────────────────────────────────────────┤
│  GameSystemManager (统一管理所有系统)                        │
│  ├── Managers/                                              │
│  │   ├── GameLogicManager (游戏逻辑和计算)                   │
│  │   ├── StateManager (状态管理)                            │
│  │   └── UIManager (界面管理)                               │
│  ├── Systems/                                               │
│  │   ├── AttackSystem (攻击系统)                            │
│  │   ├── LootSystem (掉落系统)                              │
│  │   └── EventSystem (事件系统)                             │
│  └── Config/                                                │
│      └── GameConfig (配置管理)                              │
├─────────────────────────────────────────────────────────────┤
│                    C++ Core Layer                          │
│  ├── BaseState (状态基类)                                   │
│  ├── HealthComponent (生命值组件)                            │
│  ├── DestructibleItem (可破坏物品)                          │
│  └── CurrsorCharacter (角色核心)                            │
└─────────────────────────────────────────────────────────────┘
```

## 📁 文件结构

```
TypeScript/
├── GameSystemManager.ts          # 系统管理器入口
├── Config/
│   └── GameConfig.ts             # 游戏配置和常量
├── Managers/
│   ├── GameLogicManager.ts       # 游戏逻辑管理
│   ├── StateManager.ts           # 状态管理
│   └── UIManager.ts              # UI管理
├── Systems/
│   ├── AttackSystem.ts           # 攻击系统
│   ├── LootSystem.ts             # 掉落系统
│   └── EventSystem.ts            # 事件系统
└── Blueprints/
    ├── Character/Player/
    │   ├── TS_CurrsorCharacter.ts     # 角色逻辑
    │   └── TS_CurrsorPlayerController.ts # 控制器逻辑
    └── UMG/Debug/
        └── TS_Debug.ts               # 调试界面
```

## 🎯 核心系统介绍

### 1. GameSystemManager
- **职责**: 统一管理所有游戏系统的生命周期
- **功能**: 初始化、更新、重置、销毁所有系统
- **使用**: `gameSystemManager.initialize()`

### 2. EventSystem
- **职责**: 提供发布-订阅模式的事件通信
- **功能**: 事件注册、发布、优先级管理、历史记录
- **使用**: `EventSystem.emit("eventName", data)`

### 3. StateManager
- **职责**: 管理角色状态转换和验证
- **功能**: 状态优先级、转换条件、状态生命周期
- **状态**: Idle, Walk, Run, Jump, Fall, Attack, RunAttack, Dash, Hurt, Dead

### 4. AttackSystem
- **职责**: 处理攻击逻辑和伤害计算
- **功能**: 攻击验证、伤害计算、连击系统、攻击反馈
- **特性**: 支持暴击、连击加成、攻击冷却

### 5. LootSystem
- **职责**: 管理物品掉落逻辑
- **功能**: 掉落表配置、概率计算、条件掉落
- **特性**: 支持稀有度、掉落历史、动态倍率

### 6. GameLogicManager
- **职责**: 核心游戏逻辑和数学计算
- **功能**: 伤害计算、经验值计算、规则验证
- **特性**: 等级加成、随机因子、平衡性调整

### 7. UIManager
- **职责**: 用户界面逻辑和显示管理
- **功能**: 伤害数字、状态显示、通知系统
- **特性**: 对象池、动画效果、性能优化

## 🔧 配置系统

### GameConfig
集中管理所有游戏配置参数：

```typescript
// 伤害配置
GameConfig.DAMAGE_MULTIPLIERS.CRITICAL  // 2.0 暴击倍率
GameConfig.ATTACK_COOLDOWN              // 0.5 攻击冷却

// 状态优先级
GameConfig.STATE_PRIORITIES.Dead        // 100 最高优先级
GameConfig.STATE_PRIORITIES.Attack      // 70 攻击优先级

// 掉落配置
GameConfig.ITEM_DROP_RATES.RARE         // 0.08 稀有物品掉落率
```

## 🎮 使用示例

### 初始化系统
```typescript
// 在游戏开始时初始化
gameSystemManager.initialize();
```

### 处理攻击输入
```typescript
// 在控制器中
EventSystem.emit("onAttackInput", {
    attacker: playerActor,
    inputType: "primary"
});
```

### 状态转换
```typescript
// 获取状态管理器
const stateManager = gameSystemManager.getSystem("StateManager");

// 转换状态
stateManager.changeState(StateManager.STATES.ATTACK);
```

### 生成掉落
```typescript
// 获取掉落系统
const lootSystem = gameSystemManager.getSystem("LootSystem");

// 生成掉落
lootSystem.generateLoot(sourceActor, "DestructibleItem");
```

## 🐛 调试功能

### 调试界面 (TS_Debug)
- 显示新旧架构的状态对比
- 实时系统信息监控
- 测试功能按钮
- 系统性能统计

### 调试方法
```typescript
// 打印系统状态
gameSystemManager.debugPrintStatus();

// 获取系统信息
const systemStatus = gameSystemManager.getSystemStatus();

// 查看事件历史
const eventHistory = EventSystem.getEventHistory();
```

## 🔄 事件流程

### 攻击流程
1. 玩家输入 → `onAttackInput` 事件
2. AttackSystem 验证攻击条件
3. StateManager 转换到攻击状态
4. 动画通知激活碰撞盒 → `onAttackHitboxActive`
5. 碰撞检测 → 伤害计算 → `onDamageApplied`
6. UIManager 显示伤害数字和效果
7. 攻击结束 → 状态恢复

### 状态转换流程
1. 输入/条件触发状态变化请求
2. StateManager 检查转换条件和优先级
3. 执行状态退出逻辑 → `onExit{State}`
4. 更新当前状态
5. 执行状态进入逻辑 → `onEnter{State}`
6. 广播状态变化事件 → `onStateChanged`

## 🚀 优势特性

### 1. **职责分离**
- C++处理性能敏感的核心逻辑
- TypeScript处理业务逻辑和规则
- 清晰的接口边界

### 2. **事件驱动**
- 松耦合的系统间通信
- 易于扩展和维护
- 支持优先级和历史记录

### 3. **配置化**
- 集中的参数管理
- 易于调试和平衡性调整
- 支持运行时修改

### 4. **可扩展性**
- 模块化的系统设计
- 统一的管理接口
- 易于添加新功能

### 5. **调试友好**
- 完整的调试界面
- 详细的日志输出
- 实时状态监控

## 📈 性能考虑

### 1. **对象池**
- UI元素复用 (伤害数字、通知等)
- 减少GC压力

### 2. **事件优化**
- 优先级队列
- 批量处理
- 历史记录限制

### 3. **计算缓存**
- 状态持续时间缓存
- 伤害计算结果缓存

## 🔮 未来扩展

### 计划中的功能
- [ ] 技能系统集成
- [ ] 装备系统
- [ ] 成就系统
- [ ] 存档系统
- [ ] 网络同步支持

### 架构改进
- [ ] 更细粒度的组件化
- [ ] 异步任务管理
- [ ] 内存池优化
- [ ] 热重载支持

---

## 📝 开发指南

### 添加新系统
1. 在对应目录创建系统类
2. 在 GameSystemManager 中注册
3. 设置必要的事件监听
4. 更新调试界面显示

### 修改配置
1. 在 GameConfig 中添加配置项
2. 在相关系统中使用配置
3. 添加验证逻辑

### 调试技巧
1. 使用 TS_Debug 界面监控状态
2. 查看控制台日志输出
3. 使用事件历史追踪问题
4. 利用系统统计信息分析性能

这个架构提供了一个强大、灵活且易于维护的游戏系统基础，支持快速迭代和功能扩展。