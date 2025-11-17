# 卡牌Widget Pool管理系统

## 概述

这是一个基于对象池模式的卡牌Widget管理系统，用于高效地管理手牌UI的创建、显示和回收。

## 文件结构

```
TypeScript/
├── Config/
│   └── GameConfig.ts          # 游戏配置（包含卡牌系统配置）
└── UMG/
    └── Card/
        ├── TS_CardWidgetPool.ts    # Widget对象池管理类
        ├── TS_CardMainUI.ts        # 卡牌主UI（集成Widget Pool）
        ├── TS_Card.ts              # 卡牌面板
        └── TS_Panel_CardMainUI.ts  # 卡牌主面板
```

## 核心功能

### 1. Widget Pool（TS_CardWidgetPool.ts）

对象池管理类，负责Widget的创建、回收和复用。

**主要方法：**
- `Acquire()`: 从对象池获取一个Widget
- `Release(widget)`: 将Widget归还到对象池
- `ReleaseAll()`: 释放所有激活的Widget
- `Clear()`: 清空对象池
- `GetActiveCount()`: 获取激活的Widget数量
- `GetPoolCount()`: 获取池中可用的Widget数量

### 2. 手牌管理（TS_CardMainUI.ts）

集成了Widget Pool的卡牌主UI类。

**主要方法：**

#### 添加手牌
```typescript
public AddCard(numCards: number = 1): void
```
- 从抽牌堆随机抽取指定数量的卡牌
- 自动检查手牌上限
- 从Widget Pool获取Widget并显示
- 更新UI布局

#### 弃牌
```typescript
public DiscardCard(cardIndex: number): void
public DiscardCardByInfo(cardInfo: S_CardInfo): void
```
- 将手牌移动到弃牌堆
- 自动回收Widget到对象池
- 更新UI布局

#### 清空手牌
```typescript
public ClearHand(): void
```
- 清空所有手牌
- 自动回收所有Widget

#### 洗牌
```typescript
public ShuffleDiscardPile(): void
```
- 将弃牌堆的牌放回抽牌堆
- 使用Fisher-Yates算法洗牌

#### 获取状态
```typescript
public GetHandSize(): number
public GetPoolStats(): string
```
- 获取当前手牌数量
- 获取Widget Pool状态（用于调试）

## 配置说明

在 `GameConfig.ts` 中的 `CARD_CONFIG` 配置：

```typescript
static readonly CARD_CONFIG = {
    // Widget Pool配置
    WIDGET_POOL_SIZE: 10,           // Widget池初始大小
    MAX_HAND_CARDS: 10,             // 最大手牌数量
    INITIAL_DRAW_COUNT: 5,          // 初始抽牌数量
    
    // 卡牌布局配置
    CARD_SPACING: 120,              // 卡牌间距
    CARD_SCALE: 1.0,                // 卡牌缩放
    HAND_CURVE_HEIGHT: 50,          // 手牌弧度高度
    
    // 卡牌动画配置
    CARD_DRAW_DURATION: 0.3,        // 抽牌动画时长
    CARD_DISCARD_DURATION: 0.2,     // 弃牌动画时长
    CARD_HOVER_SCALE: 1.1,          // 悬停时缩放
};
```

## 使用示例

### 初始化
Widget Pool在 `Construct()` 时自动初始化：
```typescript
Construct() {
    // 初始化Widget Pool
    this.widgetPool = new CardWidgetPool(this);
    this.InitCard();
}
```

### 抽牌
```typescript
// 抽1张牌
this.AddCard();

// 抽3张牌
this.AddCard(3);
```

### 弃牌
```typescript
// 通过索引弃牌
this.DiscardCard(0);

// 通过卡牌信息弃牌
this.DiscardCardByInfo(cardInfo);
```

### 清空手牌
```typescript
this.ClearHand();
```

### 洗牌
```typescript
// 将弃牌堆洗回抽牌堆
this.ShuffleDiscardPile();
```

### 调试
```typescript
// 获取手牌数量
const handSize = this.GetHandSize();
console.log(`当前手牌数: ${handSize}`);

// 获取Widget Pool状态
const poolStats = this.GetPoolStats();
console.log(poolStats); // 输出: "激活: 5, 池中: 5"
```

## 性能优化

### 对象池优势
1. **减少GC压力**: 复用Widget而不是频繁创建销毁
2. **提高性能**: 预创建Widget，避免运行时创建开销
3. **内存管理**: 控制Widget数量上限，防止内存泄漏

### 最佳实践
1. 根据实际需求调整 `WIDGET_POOL_SIZE`
2. 及时调用 `Destruct()` 清理资源
3. 使用 `GetPoolStats()` 监控对象池状态

## 待实现功能

- [ ] 手牌布局动画（弧形排列）
- [ ] 卡牌拖拽功能
- [ ] 卡牌悬停效果
- [ ] 卡牌使用动画
- [ ] 手牌容器UI集成

## 注意事项

1. 确保在 `Destruct()` 中清理Widget Pool
2. Widget Pool大小应根据最大手牌数量设置
3. 当前版本使用 `AddToViewport()` 添加Widget，实际项目中应该添加到专门的手牌容器中
4. `UpdateHandCardsLayout()` 方法需要根据实际UI结构实现具体的布局逻辑
