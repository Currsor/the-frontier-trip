# 卡牌出场插值动画系统

## 概述

卡牌出场插值动画系统（`CardAnimationSystem`）是一个专门用于管理卡牌从起始位置（startHook）平滑过渡到目标位置的动画系统。该系统支持：

- ✅ 从指定的startHook位置开始动画
- ✅ 平滑插值到目标位置
- ✅ 动态调整所有正在动画的卡牌的终点位置
- ✅ 支持位置、旋转、缩放的同步动画
- ✅ 使用缓动函数实现自然的动画效果

## 核心特性

### 1. 从startHook开始动画

卡牌从抽牌堆按钮（`Widget_CardListButton`）的位置开始，以较小的缩放（0.5倍）出现，然后平滑过渡到手牌区域。

```typescript
// 自动从Widget_CardListButton位置开始
this.animationSystem.StartCardAnimation(
    cardWidget,
    startHookWidget,  // 通常是 Widget_CardListButton
    targetPos,
    targetRotation,
    targetScale,
    duration
);
```

### 2. 插值到目标位置

系统使用 **EaseOutCubic** 缓动函数，实现平滑的动画效果：

- **位置插值**：从startHook位置到目标位置
- **旋转插值**：从0度到目标旋转角度
- **缩放插值**：从0.5倍到目标缩放（通常是1.0倍）

### 3. 动态调整终点

当新增卡牌时，所有正在动画的卡牌会自动调整它们的目标位置，确保最终布局正确：

```typescript
// 当新增卡牌时，自动重新计算所有卡牌的目标位置
this.UpdateHandCardsLayoutWithAnimation();

// 系统会自动调用
this.animationSystem.UpdateAllTargetPositions(newTargets);
```

## 文件结构

```
TypeScript/UMG/Card/
├── TS_CardAnimationSystem.ts      # 动画系统核心
├── TS_CardMainUI.ts                # 主UI，集成动画系统
├── TS_CardWidgetPool.ts            # Widget对象池
├── TS_Card.ts                      # 卡牌Widget
└── README_CardAnimationSystem.md   # 本文档
```

## 使用方法

### 初始化

在 `TS_CardMainUI` 的 `Construct()` 方法中自动初始化：

```typescript
Construct() {
    // 初始化Widget Pool
    this.widgetPool = new CardWidgetPool(this);
    
    // 初始化动画系统
    this.animationSystem = new CardAnimationSystem(this);
    
    this.InitCard();
}
```

### 抽牌时自动播放动画

当调用 `AddCard()` 时，新卡牌会自动播放出场动画：

```typescript
// 抽1张牌（带动画）
this.AddCard();

// 抽3张牌（每张都带动画）
this.AddCard(3);
```

### 动画流程

1. **创建Widget**：从对象池获取卡牌Widget
2. **添加到容器**：将Widget添加到HandCardsContainer
3. **启动动画**：
   - 从 `Widget_CardListButton` 位置开始
   - 初始缩放为0.5倍
   - 插值到目标位置、旋转和缩放
4. **动态调整**：如果在动画期间新增卡牌，所有卡牌的目标位置会自动更新

## 配置参数

在 `GameConfig.ts` 中的 `CARD_CONFIG` 配置：

```typescript
static readonly CARD_CONFIG = {
    // 动画配置
    CARD_DRAW_DURATION: 0.5,              // 抽牌动画时长（秒）
    CARD_DISCARD_DURATION: 0.2,           // 弃牌动画时长（秒）
    CARD_ANIMATION_START_SCALE: 0.5,      // 动画起始缩放
    CARD_ANIMATION_EASING: 'EaseOutCubic', // 缓动函数类型
    
    // 布局配置
    CARD_SPACING: 120 * 1.2,              // 卡牌间距
    CARD_SCALE: 1.0,                      // 卡牌缩放
    HAND_CURVE_HEIGHT: -50,               // 手牌弧度高度
};
```

### 参数说明

| 参数 | 类型 | 默认值 | 说明 |
|------|------|--------|------|
| `CARD_DRAW_DURATION` | number | 0.5 | 抽牌动画持续时间（秒） |
| `CARD_ANIMATION_START_SCALE` | number | 0.5 | 动画开始时的缩放比例 |
| `CARD_SPACING` | number | 144 | 卡牌之间的间距（像素） |
| `HAND_CURVE_HEIGHT` | number | -50 | 手牌弧形高度（负值向上弯曲） |

## API 参考

### CardAnimationSystem

#### 构造函数

```typescript
constructor(owningWidget: UE.UserWidget)
```

#### 主要方法

##### StartCardAnimation

启动卡牌出场动画：

```typescript
public StartCardAnimation(
    widget: UE.Game.UI.Blueprints.Cards.Widget_CardCell.Widget_CardCell_C,
    startHookWidget: UE.Widget,
    targetPos: UE.Vector2D,
    targetRotation: number,
    targetScale: UE.Vector2D,
    duration: number = GameConfig.CARD_CONFIG.CARD_DRAW_DURATION
): void
```

**参数：**
- `widget`: 要动画的卡牌Widget
- `startHookWidget`: 起始位置的Widget（通常是抽牌堆按钮）
- `targetPos`: 目标位置
- `targetRotation`: 目标旋转角度
- `targetScale`: 目标缩放
- `duration`: 动画时长（秒）

##### UpdateAllTargetPositions

动态更新所有正在动画的卡牌的目标位置：

```typescript
public UpdateAllTargetPositions(
    newTargets: Map<Widget, {pos: Vector2D, rotation: number, scale: Vector2D}>
): void
```

**使用场景：**
- 当新增卡牌时，需要重新排列所有卡牌
- 当移除卡牌时，需要调整剩余卡牌的位置

##### IsAnimating

检查指定Widget是否正在动画：

```typescript
public IsAnimating(widget: Widget): boolean
```

##### StopAnimation

停止指定Widget的动画：

```typescript
public StopAnimation(widget: Widget): void
```

##### StopAllAnimations

停止所有动画：

```typescript
public StopAllAnimations(): void
```

##### Cleanup

清理资源（在Destruct时调用）：

```typescript
public Cleanup(): void
```

## 工作原理

### 动画更新循环

系统使用 `setInterval` 实现约60fps的更新循环：

```typescript
// 每16ms更新一次（约60fps）
setInterval(() => {
    this.UpdateAnimations();
}, 16);
```

### 插值计算

使用 **EaseOutCubic** 缓动函数：

```typescript
private EaseOutCubic(t: number): number {
    return 1 - Math.pow(1 - t, 3);
}
```

这个函数提供了"快速开始，缓慢结束"的动画效果，让卡牌移动更自然。

### 动态目标调整

当新增卡牌时：

1. 重新计算所有卡牌的目标位置
2. 调用 `UpdateAllTargetPositions()` 更新正在动画的卡牌
3. 正在动画的卡牌会平滑过渡到新的目标位置
4. 已完成动画的卡牌直接设置到新位置

## 示例场景

### 场景1：初始抽牌

```typescript
// 游戏开始时抽5张牌
this.AddCard(5);

// 每张牌都会：
// 1. 从抽牌堆位置开始
// 2. 以0.5倍缩放出现
// 3. 在0.5秒内插值到目标位置
// 4. 最终缩放到1.0倍
```

### 场景2：动态新增卡牌

```typescript
// 已有3张牌在手
// 再抽2张牌
this.AddCard(2);

// 新增的2张牌会播放出场动画
// 已有的3张牌会平滑移动到新位置（如果还在动画中）
```

### 场景3：弃牌后重新排列

```typescript
// 弃掉一张牌
this.DiscardCard(1);

// 剩余卡牌会重新排列
// 正在动画的卡牌会平滑过渡到新位置
```

## 性能优化

### 自动停止更新循环

当没有活动动画时，系统会自动停止更新循环，节省CPU资源：

```typescript
if (!hasActiveAnimations) {
    this.StopUpdateLoop();
    this.CleanupCompletedAnimations();
}
```

### 对象池复用

配合 `CardWidgetPool` 使用，避免频繁创建销毁Widget：

```typescript
// 从对象池获取
const cardWidget = this.widgetPool.Acquire();

// 使用完毕后归还
this.widgetPool.Release(cardWidget);
```

## 调试

### 检查动画状态

```typescript
// 检查Widget是否正在动画
if (this.animationSystem.IsAnimating(widget)) {
    console.log('卡牌正在动画中');
}

// 获取对象池状态
console.log(this.GetPoolStats());
// 输出: "激活: 5, 池中: 5"
```

### 常见问题

#### 1. 动画不播放

**原因：** 未找到 `Widget_CardListButton`

**解决：** 确保蓝图中存在名为 `Widget_CardListButton` 的Widget

#### 2. 动画卡顿

**原因：** 更新频率过高或设备性能不足

**解决：** 调整更新间隔或减少同时动画的卡牌数量

#### 3. 目标位置不正确

**原因：** 布局计算错误

**解决：** 检查 `CalculateCardTargetPositions()` 的计算逻辑

## 扩展功能

### 自定义缓动函数

可以添加更多缓动函数：

```typescript
// EaseInOutQuad
private EaseInOutQuad(t: number): number {
    return t < 0.5 
        ? 2 * t * t 
        : 1 - Math.pow(-2 * t + 2, 2) / 2;
}

// EaseOutBounce
private EaseOutBounce(t: number): number {
    const n1 = 7.5625;
    const d1 = 2.75;
    
    if (t < 1 / d1) {
        return n1 * t * t;
    } else if (t < 2 / d1) {
        return n1 * (t -= 1.5 / d1) * t + 0.75;
    } else if (t < 2.5 / d1) {
        return n1 * (t -= 2.25 / d1) * t + 0.9375;
    } else {
        return n1 * (t -= 2.625 / d1) * t + 0.984375;
    }
}
```

### 添加动画事件

可以添加动画开始、进行中、完成的回调：

```typescript
interface CardAnimationData {
    // ... 现有字段
    onStart?: () => void;
    onUpdate?: (progress: number) => void;
    onComplete?: () => void;
}
```

## 总结

卡牌出场插值动画系统提供了一个完整的解决方案，用于管理卡牌的出场动画。它具有以下优势：

- ✅ **易于使用**：自动集成到抽牌流程中
- ✅ **性能优化**：自动停止不必要的更新循环
- ✅ **灵活配置**：通过GameConfig轻松调整参数
- ✅ **动态适应**：支持动态调整目标位置
- ✅ **平滑自然**：使用缓动函数实现自然的动画效果

通过这个系统，可以轻松实现专业级的卡牌游戏UI动画效果。
