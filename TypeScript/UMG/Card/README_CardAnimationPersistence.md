# 卡牌动画系统持久化方案

## 问题背景

在之前的实现中，`CardAnimationSystem` 是在 `TS_CardMainUI` 的 `Construct()` 方法中创建的局部实例。这导致以下问题：

1. **易被卸载**：当 UI 被销毁时，动画系统也会被销毁
2. **状态丢失**：正在进行的动画会被中断
3. **资源浪费**：每次创建 UI 都需要重新初始化动画系统

## 解决方案

### 1. CardAnimationManager 单例

创建了一个持久化的 `CardAnimationManager` 单例类，负责管理所有 `CardAnimationSystem` 实例：

```typescript
export class CardAnimationManager {
    private static instance: CardAnimationManager;
    private animationSystems: Map<string, CardAnimationSystem> = new Map();
    
    // 获取或创建指定UI的动画系统
    public getOrCreateAnimationSystem(uiId: string, owningWidget: UE.UserWidget): CardAnimationSystem {
        // 如果已存在，复用；否则创建新的
    }
    
    // 移除指定UI的动画系统
    public removeAnimationSystem(uiId: string, cleanup: boolean = false): void {
        // 可选择是否清理资源
    }
    
    // 清理所有动画系统
    public cleanupAll(): void {
        // 清理所有动画系统资源
    }
}
```

### 2. 集成到 GameSystemManager

将 `CardAnimationManager` 集成到游戏系统管理器中，确保其生命周期与游戏一致：

```typescript
// GameSystemManager.ts
private initializeSystems(): void {
    // ... 其他系统初始化
    
    // 卡牌动画管理器
    const cardAnimationManager = CardAnimationManager.getInstance();
    this.systems.set("CardAnimationManager", cardAnimationManager);
    console.log("卡牌动画管理器已初始化");
}
```

### 3. 更新 TS_CardMainUI

修改 `TS_CardMainUI` 以使用持久化的动画管理器：

```typescript
export class TS_CardMainUI implements TS_CardMainUI {
    private animationSystem: CardAnimationSystem | null = null;
    private readonly UI_ID = 'CardMainUI';
    
    Construct() {
        // 从管理器获取或创建动画系统（持久化）
        this.animationSystem = CardAnimationManager.getInstance()
            .getOrCreateAnimationSystem(this.UI_ID, this);
    }
    
    Destruct() {
        // 注意：不清理动画系统，保持持久化
        // 只清除引用，动画系统由CardAnimationManager管理
        this.animationSystem = null;
        console.log('[TS_CardMainUI] 动画系统引用已清除（系统保持持久化）');
    }
}
```

## 核心特性

### 1. 持久化存储

- 动画系统实例存储在 `CardAnimationManager` 中
- 不会随 UI 的销毁而被清理
- 支持 UI 重新创建时复用动画系统

### 2. 自动引用更新

当 UI 重新创建时，`CardAnimationSystem` 会自动更新 `owningWidget` 引用：

```typescript
// CardAnimationSystem.ts
public updateOwningWidget(owningWidget: UE.UserWidget): void {
    this.owningWidget = owningWidget;
    console.log("[CardAnimationSystem] owningWidget引用已更新");
}
```

### 3. 灵活的清理策略

提供多种清理选项：

```typescript
// 选项1：只移除引用，保留动画系统（默认）
manager.removeAnimationSystem('CardMainUI', false);

// 选项2：移除并清理动画系统
manager.removeAnimationSystem('CardMainUI', true);

// 选项3：清理所有动画系统
manager.cleanupAll();
```

## 使用方法

### 基本使用

```typescript
// 1. 在UI的Construct中获取动画系统
Construct() {
    this.animationSystem = CardAnimationManager.getInstance()
        .getOrCreateAnimationSystem('MyUIId', this);
}

// 2. 正常使用动画系统
this.animationSystem.StartCardAnimation(...);

// 3. 在Destruct中只清除引用
Destruct() {
    this.animationSystem = null;
}
```

### 手动清理（可选）

如果需要完全清理某个UI的动画系统：

```typescript
// 在适当的时机（如游戏结束、场景切换等）
CardAnimationManager.getInstance().removeAnimationSystem('MyUIId', true);
```

### 全局清理

在游戏系统重置时：

```typescript
// GameSystemManager.ts
public resetAllSystems(): void {
    const cardAnimationManager = this.getSystem("CardAnimationManager") as CardAnimationManager;
    if (cardAnimationManager) {
        cardAnimationManager.cleanupAll();
    }
}
```

## 优势

### 1. 避免被卸载

- ✅ 动画系统不会随 UI 销毁而被清理
- ✅ 正在进行的动画不会被中断
- ✅ 动画状态得以保留

### 2. 性能优化

- ✅ 避免重复创建和销毁动画系统
- ✅ 减少内存分配和垃圾回收
- ✅ 复用已有的动画系统实例

### 3. 更好的控制

- ✅ 集中管理所有动画系统
- ✅ 灵活的清理策略
- ✅ 便于调试和监控

### 4. 统一管理

- ✅ 集成到 GameSystemManager
- ✅ 与其他游戏系统一致的生命周期
- ✅ 统一的初始化和清理流程

## 注意事项

### 1. UI ID 唯一性

确保每个 UI 使用唯一的 ID：

```typescript
private readonly UI_ID = 'CardMainUI'; // 必须唯一
```

### 2. 引用更新

当 UI 重新创建时，`owningWidget` 引用会自动更新，无需手动处理。

### 3. 内存管理

虽然动画系统持久化，但在不需要时应该清理：

```typescript
// 场景切换时清理
CardAnimationManager.getInstance().cleanupAll();
```

### 4. 调试信息

可以通过统计信息查看当前状态：

```typescript
const stats = CardAnimationManager.getInstance().getStats();
console.log('动画系统数量:', stats.totalSystems);
console.log('UI列表:', stats.systems);
```

## 迁移指南

### 从旧版本迁移

如果你的代码使用了旧的局部动画系统：

**旧代码：**
```typescript
export class MyUI {
    private animationSystem: CardAnimationSystem | null = null;
    
    Construct() {
        this.animationSystem = new CardAnimationSystem(this);
    }
    
    Destruct() {
        if (this.animationSystem) {
            this.animationSystem.Cleanup();
            this.animationSystem = null;
        }
    }
}
```

**新代码：**
```typescript
export class MyUI {
    private animationSystem: CardAnimationSystem | null = null;
    private readonly UI_ID = 'MyUI'; // 添加唯一ID
    
    Construct() {
        // 从管理器获取
        this.animationSystem = CardAnimationManager.getInstance()
            .getOrCreateAnimationSystem(this.UI_ID, this);
    }
    
    Destruct() {
        // 只清除引用，不清理系统
        this.animationSystem = null;
    }
}
```

## 测试建议

### 1. 基本功能测试

- ✅ UI 创建时动画系统正常工作
- ✅ UI 销毁后动画系统仍然存在
- ✅ UI 重新创建时复用动画系统

### 2. 引用更新测试

- ✅ UI 重新创建后 owningWidget 引用正确更新
- ✅ 动画在新的 UI 实例上正常工作

### 3. 清理测试

- ✅ removeAnimationSystem 正确清理
- ✅ cleanupAll 清理所有系统
- ✅ 清理后不影响其他系统

### 4. 性能测试

- ✅ 多次创建/销毁 UI 不会导致内存泄漏
- ✅ 动画系统复用减少了初始化开销

## 总结

通过引入 `CardAnimationManager` 单例和集成到 `GameSystemManager`，我们实现了卡牌动画系统的持久化管理。这不仅解决了动画系统易被卸载的问题，还提供了更好的性能和更灵活的控制方式。
