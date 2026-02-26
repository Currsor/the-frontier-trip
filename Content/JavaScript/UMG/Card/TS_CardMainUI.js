"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.TS_CardMainUI = void 0;
const UE = require("ue");
const puerts_1 = require("puerts");
const TS_CardWidgetPool_1 = require("./TS_CardWidgetPool");
const TS_CardAnimationSystem_1 = require("./TS_CardAnimationSystem");
const GameConfig_1 = require("../../Config/GameConfig");
const EventSystem_1 = require("../../Systems/EventSystem");
const uclass = UE.Class.Load("/Game/UI/Blueprints/Cards/Widget_CardMainUI.Widget_CardMainUI_C");
const jsClass = puerts_1.blueprint.tojs(uclass);
class TS_CardMainUI {
    static pawn;
    static PlayerState;
    // Widget Pool管理器
    widgetPool = null;
    // 卡牌动画系统（从管理器获取）
    animationSystem = null;
    UI_ID = 'CardMainUI';
    // 正在拖动的卡牌信息（用于跟踪卡牌使用）
    draggingCardInfo = null;
    draggingCardWidget = null;
    Construct() {
        TS_CardMainUI.pawn = this.GetOwningPlayerPawn();
        TS_CardMainUI.PlayerState = TS_CardMainUI.pawn.PlayerState;
        // 初始化Widget Pool
        this.widgetPool = new TS_CardWidgetPool_1.CardWidgetPool(this);
        // 验证Widget Pool是否成功初始化
        if (!this.widgetPool || !this.widgetPool.IsInitialized()) {
            console.error('TS_CardMainUI: Widget Pool初始化失败');
            return;
        }
        // 从管理器获取或创建动画系统（持久化）
        this.animationSystem = TS_CardAnimationSystem_1.CardAnimationManager.getInstance().getOrCreateAnimationSystem(this.UI_ID, this);
        EventSystem_1.EventSystem.subscribe("UpdateMana", this.onUpdateMana.bind(this));
        EventSystem_1.EventSystem.subscribe("CardDragStart", this.onCardDragStart.bind(this));
        EventSystem_1.EventSystem.subscribe("CardDragEnd", this.onCardDragEnd.bind(this));
        this.InitCard();
    }
    Destruct() {
        // 取消事件订阅
        EventSystem_1.EventSystem.unsubscribe("UpdateMana", this.onUpdateMana.bind(this));
        EventSystem_1.EventSystem.unsubscribe("CardDragStart", this.onCardDragStart.bind(this));
        EventSystem_1.EventSystem.unsubscribe("CardDragEnd", this.onCardDragEnd.bind(this));
        // 注意：不清理动画系统，保持持久化
        // 只清除引用，动画系统由CardAnimationManager管理
        this.animationSystem = null;
        console.log('[TS_CardMainUI] 动画系统引用已清除（系统保持持久化）');
        // 清理Widget Pool
        if (this.widgetPool) {
            this.widgetPool.Clear();
            this.widgetPool = null;
        }
        this.handCardWidgets.Empty();
        // 清理拖动状态
        this.draggingCardInfo = null;
        this.draggingCardWidget = null;
    }
    InitCard() {
        const deckCardNames = TS_CardMainUI.PlayerState.DeckCardNames;
        for (let i = 0; i < deckCardNames.Num(); i++) {
            const cardName = deckCardNames.Get(i);
            const cardInfo = (TS_CardMainUI.pawn.GetDataFromName(cardName));
            this.drawPile.Add(cardInfo);
        }
        this.AddCard(GameConfig_1.GameConfig.CARD_CONFIG.INITIAL_DRAW_COUNT);
        this.UpdateDrawPileUI();
        this.UpdateDiscardPileUI();
    }
    onUpdateMana(data) {
        this.Widget_PlayerStatusIndicator.StatText.SetText(data.Amount.toString());
        // 如果有正在拖动的卡牌，说明卡牌使用成功，将其加入弃牌堆
        if (this.draggingCardInfo) {
            console.log('[TS_CardMainUI] 卡牌使用成功，加入弃牌堆:', this.draggingCardInfo.Name);
            this.discardPile.Add(this.draggingCardInfo);
            // 更新弃牌堆UI
            this.UpdateDiscardPileUI();
            // 清除拖动状态
            this.draggingCardInfo = null;
            this.draggingCardWidget = null;
        }
    }
    /**
     * 卡牌开始拖动事件处理
     */
    onCardDragStart(data) {
        const card = data.card;
        if (!card || !card.cardInfo) {
            console.warn('[TS_CardMainUI] onCardDragStart: 无效的卡牌');
            return;
        }
        console.log('[TS_CardMainUI] 卡牌开始拖动:', card.cardInfo.Name);
        // 记录原始位置、旋转、缩放和层级
        const slot = card.Slot;
        if (slot) {
            card.originalPosition = slot.GetPosition();
            card.originalRotation = card.RenderTransform.Angle;
            card.originalScale = card.RenderTransform.Scale;
            card.originalZOrder = slot.GetZOrder();
            console.log('[TS_CardMainUI] 记录原始状态:', {
                position: card.originalPosition,
                rotation: card.originalRotation,
                scale: card.originalScale,
                zOrder: card.originalZOrder
            });
        }
        // 记录正在拖动的卡牌信息
        this.draggingCardInfo = card.cardInfo;
        this.draggingCardWidget = card;
        // 从手牌数据中移除
        for (let i = 0; i < this.handCards.Num(); i++) {
            if (this.handCards.Get(i) === card.cardInfo) {
                this.handCards.RemoveAt(i);
                break;
            }
        }
        // 归还Widget到对象池
        if (this.widgetPool && this.widgetPool.IsInitialized()) {
            this.widgetPool.Release(card);
            this.handCardWidgets.Remove(card);
        }
        // 更新手牌布局
        this.UpdateHandCardsLayoutWithAnimation();
    }
    /**
     * 卡牌拖动结束事件处理
     */
    onCardDragEnd(data) {
        const card = data.card;
        const success = data.success;
        console.log('[TS_CardMainUI] 卡牌拖动结束:', card?.cardInfo?.Name, '成功:', success);
        // 如果拖动未成功（没有放到Widget_PlayingCardArea），还原卡牌到原始位置
        if (!success && this.draggingCardInfo && card) {
            console.log('[TS_CardMainUI] 卡牌拖动失败，还原到原始位置');
            // 将卡牌放回手牌数据
            this.handCards.Add(this.draggingCardInfo);
            // 还原卡牌到原始位置
            this.RestoreCardToOriginalPosition(card);
            // 清除拖动状态
            this.draggingCardInfo = null;
            this.draggingCardWidget = null;
        }
        // 如果成功，等待UpdateMana事件来处理弃牌
    }
    /**
     * 还原卡牌到原始位置（使用动画）
     */
    RestoreCardToOriginalPosition(card) {
        if (!card.originalPosition) {
            console.warn('[TS_CardMainUI] 无法还原卡牌：未记录原始位置');
            // 如果没有记录原始位置，重新创建Widget
            if (card.cardInfo) {
                this.CreateHandCardWidget(card.cardInfo);
            }
            return;
        }
        // 将卡牌重新添加到手牌容器
        const container = this.HandCardsContainer;
        if (container) {
            container.AddChildToCanvas(card);
        }
        // 重新添加到handCardWidgets映射
        if (card.cardInfo) {
            this.handCardWidgets.Add(card, card.cardInfo);
        }
        // 使用动画系统还原到原始位置
        if (this.animationSystem && card.originalPosition) {
            console.log('[TS_CardMainUI] 启动还原动画到原始位置:', card.originalPosition);
            // 确保卡牌大小和层级正确设置
            const slot = card.Slot;
            if (slot) {
                slot.SetAlignment(new UE.Vector2D(0.5, 0.5));
                slot.SetSize(new UE.Vector2D(GameConfig_1.GameConfig.CARD_CONFIG.CARD_WIDTH, GameConfig_1.GameConfig.CARD_CONFIG.CARD_HEIGHT));
                slot.SetAnchors(new UE.Anchors(new UE.Vector2D(0.5, 0.5), new UE.Vector2D(0.5, 0.5)));
                slot.SetZOrder(card.originalZOrder);
            }
            // 启动还原动画
            this.animationSystem.StartRepositionAnimation(card, card.originalPosition, card.originalRotation, card.originalScale || new UE.Vector2D(GameConfig_1.GameConfig.CARD_CONFIG.CARD_SCALE, GameConfig_1.GameConfig.CARD_CONFIG.CARD_SCALE), 0.3 // 还原动画时长
            );
            // 清除原始位置记录
            card.originalPosition = null;
            card.originalScale = null;
        }
    }
    // 抽取手牌
    AddCard(numCards = 1) {
        if (!this.widgetPool || !this.widgetPool.IsInitialized()) {
            console.warn('AddCard: Widget Pool未初始化，尝试创建...');
            this.widgetPool = new TS_CardWidgetPool_1.CardWidgetPool(this);
            if (!this.widgetPool || !this.widgetPool.IsInitialized()) {
                console.error('AddCard: Widget Pool创建失败');
                return;
            }
        }
        if (this.drawPile.Num() === 0)
            return;
        // 检查手牌上限
        const maxCards = GameConfig_1.GameConfig.CARD_CONFIG.MAX_HAND_CARDS;
        const currentHandSize = this.handCards.Num();
        if (currentHandSize >= maxCards) {
            console.warn(`手牌已达上限: ${maxCards}`);
            return;
        }
        // 计算实际可抽取的牌数
        const cardsToAdd = Math.min(numCards, this.drawPile.Num(), maxCards - currentHandSize);
        for (let i = 0; i < cardsToAdd; i++) {
            // 从抽牌堆随机抽取
            const randomIndex = Math.floor(Math.random() * this.drawPile.Num());
            const cardInfo = this.drawPile.Get(randomIndex);
            this.drawPile.RemoveAt(randomIndex);
            // 添加到手牌数据
            this.handCards.Add(cardInfo);
            // 从Widget Pool获取Widget并显示
            this.CreateHandCardWidget(cardInfo);
        }
        // 更新UI
        this.UpdateDrawPileUI();
        this.UpdateHandCardsLayoutWithAnimation();
    }
    /**
     * 创建手牌Widget并添加到UI
     */
    CreateHandCardWidget(cardInfo) {
        if (!this.widgetPool || !this.widgetPool.IsInitialized()) {
            console.warn('CreateHandCardWidget: Widget Pool未初始化，尝试创建...');
            this.widgetPool = new TS_CardWidgetPool_1.CardWidgetPool(this);
            if (!this.widgetPool || !this.widgetPool.IsInitialized()) {
                console.error('CreateHandCardWidget: Widget Pool创建失败');
                return;
            }
        }
        // 从对象池获取Widget
        const cardWidget = this.widgetPool.Acquire();
        if (!cardWidget) {
            console.error('无法从Widget Pool获取卡牌Widget');
            return;
        }
        // 设置卡牌数据
        this.SetupCardWidget(cardWidget, cardInfo);
        // 添加到手牌容器
        if (this.HandCardsContainer) {
            const container = this.HandCardsContainer;
            container.AddChildToCanvas(cardWidget);
        }
        // 记录映射关系（键为Widget，值为CardInfo）
        this.handCardWidgets.Add(cardWidget, cardInfo);
        // 计算新卡牌的目标位置（此时handCardWidgets已包含新卡牌）
        const targetPositions = this.CalculateCardTargetPositions();
        const target = targetPositions.get(cardWidget);
        if (!target) {
            console.warn('无法计算卡牌目标位置');
            return;
        }
        // 启动出场动画，使用计算好的目标位置和层级
        this.StartCardEntranceAnimation(cardWidget, target.pos, target.rotation, target.scale, target.zOrder);
    }
    /**
     * 启动卡牌出场动画
     * @param cardWidget 卡牌Widget
     * @param targetPos 目标位置
     * @param targetRotation 目标旋转
     * @param targetScale 目标缩放
     * @param targetZOrder 目标层级
     */
    StartCardEntranceAnimation(cardWidget, targetPos, targetRotation, targetScale, targetZOrder) {
        if (!this.animationSystem)
            return;
        // 获取startHook（抽牌堆起始位置）
        const startHook = this.bp_StartHook;
        if (!startHook) {
            console.warn('未找到bp_StartHook，无法播放出场动画');
            return;
        }
        // 确保卡牌大小和层级正确设置
        const slot = cardWidget.Slot;
        if (slot) {
            slot.SetAlignment(new UE.Vector2D(0.5, 0.5));
            slot.SetSize(new UE.Vector2D(GameConfig_1.GameConfig.CARD_CONFIG.CARD_WIDTH, GameConfig_1.GameConfig.CARD_CONFIG.CARD_HEIGHT));
            slot.SetAnchors(new UE.Anchors(new UE.Vector2D(0.5, 0.5), new UE.Vector2D(0.5, 0.5)));
            // 设置目标层级
            slot.SetZOrder(targetZOrder);
        }
        // 启动动画
        this.animationSystem.StartCardAnimation(cardWidget, startHook, targetPos, targetRotation, targetScale, GameConfig_1.GameConfig.CARD_CONFIG.CARD_DRAW_DURATION);
    }
    /**
     * 设置卡牌Widget的数据
     */
    SetupCardWidget(widget, cardInfo) {
        // 设置动画系统引用
        if (this.animationSystem) {
            widget.animationSystem = this.animationSystem;
        }
        widget.SetData(cardInfo);
    }
    /**
     * 更新手牌布局
     */
    UpdateHandCardsLayoutWithAnimation() {
        const container = this.HandCardsContainer;
        if (!container)
            return;
        // 计算所有卡牌的目标位置
        const targetPositions = this.CalculateCardTargetPositions();
        // 如果没有卡牌需要更新，直接返回
        if (targetPositions.size === 0)
            return;
        // 更新正在动画的卡牌的目标位置
        if (this.animationSystem) {
            this.animationSystem.UpdateAllTargetPositions(targetPositions);
        }
        // 对于不在动画中的卡牌，直接设置位置
        this.UpdateCanvasLayoutDirect(targetPositions);
    }
    /**
     * 更新手牌布局（不带动画，直接设置）
     */
    UpdateHandCardsLayout() {
        const handSize = this.handCards.Num();
        if (handSize === 0)
            return;
        const container = this.HandCardsContainer;
        if (!container)
            return;
        this.UpdateCanvasLayout();
    }
    /**
     * 计算所有卡牌的目标位置
     */
    CalculateCardTargetPositions() {
        const targetMap = new Map();
        // 先收集所有有效的Widget
        const validWidgets = [];
        for (let i = 0; i < this.handCardWidgets.GetMaxIndex(); i++) {
            if (!this.handCardWidgets.IsValidIndex(i))
                continue;
            const widget = this.handCardWidgets.GetKey(i);
            if (widget) {
                validWidgets.push(widget);
            }
        }
        // 使用实际的Widget数量来计算布局
        const handSize = validWidgets.length;
        if (handSize === 0)
            return targetMap;
        const spacing = GameConfig_1.GameConfig.CARD_CONFIG.CARD_SPACING;
        const curveHeight = GameConfig_1.GameConfig.CARD_CONFIG.HAND_CURVE_HEIGHT;
        const cardScale = GameConfig_1.GameConfig.CARD_CONFIG.CARD_SCALE;
        const totalWidth = (handSize - 1) * spacing;
        const startX = -totalWidth / 2;
        // 为每个Widget计算目标位置
        for (let index = 0; index < validWidgets.length; index++) {
            const widget = validWidgets[index];
            // 计算 X 位置
            const x = startX + index * spacing;
            // 计算 Y 位置（弧形效果）
            const normalizedX = (index - (handSize - 1) / 2) / Math.max(handSize - 1, 1);
            const y = -Math.abs(normalizedX) * curveHeight;
            // 计算旋转角度
            const rotation = normalizedX * 5; // 最大旋转 5 度
            // 计算层级：左边的卡牌层级低，右边的卡牌层级高
            const zOrder = index;
            targetMap.set(widget, {
                pos: new UE.Vector2D(x, y),
                rotation: rotation,
                scale: new UE.Vector2D(cardScale, cardScale),
                zOrder: zOrder
            });
        }
        return targetMap;
    }
    /**
     * 直接更新Canvas布局（对于不在动画中的卡牌，启动插值动画）
     */
    UpdateCanvasLayoutDirect(targetPositions) {
        const cardWidth = GameConfig_1.GameConfig.CARD_CONFIG.CARD_WIDTH;
        const cardHeight = GameConfig_1.GameConfig.CARD_CONFIG.CARD_HEIGHT;
        for (const [widget, target] of targetPositions) {
            // 确保卡牌大小和层级正确设置
            const slot = widget.Slot;
            if (slot) {
                slot.SetAlignment(new UE.Vector2D(0.5, 0.5));
                slot.SetSize(new UE.Vector2D(cardWidth, cardHeight));
                slot.SetAnchors(new UE.Anchors(new UE.Vector2D(0.5, 0.5), new UE.Vector2D(0.5, 0.5)));
                // 设置层级：左边卡牌层级低，右边卡牌层级高
                slot.SetZOrder(target.zOrder);
            }
            // 如果卡牌正在动画中，跳过（已经在UpdateAllTargetPositions中更新了目标）
            if (this.animationSystem && this.animationSystem.IsAnimating(widget)) {
                continue;
            }
            // 对于不在动画的卡牌，启动插值动画到新位置
            if (this.animationSystem) {
                this.animationSystem.StartRepositionAnimation(widget, target.pos, target.rotation, target.scale, 0.3 // 重新定位动画时长
                );
            }
        }
    }
    /**
     * 更新 Canvas Panel 中的卡牌布局（弧形排列）
     */
    UpdateCanvasLayout() {
        const container = this.HandCardsContainer;
        if (!container)
            return;
        const handSize = this.handCards.Num();
        const spacing = GameConfig_1.GameConfig.CARD_CONFIG.CARD_SPACING;
        const curveHeight = GameConfig_1.GameConfig.CARD_CONFIG.HAND_CURVE_HEIGHT;
        const cardScale = GameConfig_1.GameConfig.CARD_CONFIG.CARD_SCALE;
        const cardWidth = GameConfig_1.GameConfig.CARD_CONFIG.CARD_WIDTH;
        const cardHeight = GameConfig_1.GameConfig.CARD_CONFIG.CARD_HEIGHT;
        // 计算总宽度和起始位置
        const totalWidth = (handSize - 1) * spacing;
        const startX = -totalWidth / 2;
        // 遍历所有手牌 Widget 并设置位置
        let index = 0;
        for (let i = 0; i < this.handCardWidgets.GetMaxIndex(); i++) {
            if (!this.handCardWidgets.IsValidIndex(i))
                continue;
            const widget = this.handCardWidgets.GetKey(i);
            if (!widget)
                continue;
            // 计算 X 位置
            const x = startX + index * spacing;
            // 计算 Y 位置（弧形效果）
            const normalizedX = (index - (handSize - 1) / 2) / Math.max(handSize - 1, 1);
            const y = -Math.abs(normalizedX) * curveHeight;
            // 计算旋转角度
            const rotation = normalizedX * 5; // 最大旋转 5 度
            // 获取 Widget 的 Slot 并设置属性
            const slot = widget.Slot;
            if (slot) {
                // 设置位置
                slot.SetPosition(new UE.Vector2D(x, y));
                // 设置对齐方式（0.5, 0.5 表示中心对齐）
                slot.SetAlignment(new UE.Vector2D(0.5, 0.5));
                // **关键：设置卡牌大小**
                slot.SetSize(new UE.Vector2D(cardWidth, cardHeight));
                // 设置锚点（可选，使用中心锚点）
                slot.SetAnchors(new UE.Anchors(new UE.Vector2D(0.5, 0.5), new UE.Vector2D(0.5, 0.5)));
            }
            // 设置缩放和旋转
            widget.SetRenderScale(new UE.Vector2D(cardScale, cardScale));
            widget.SetRenderTransformAngle(rotation);
            index++;
        }
    }
    /**
     * 更新抽牌堆UI显示
     */
    UpdateDrawPileUI() {
        if (this.Widget_CardListButton && this.Widget_CardListButton.AmountText) {
            this.Widget_CardListButton.AmountText.SetText(this.drawPile.Num().toString());
        }
    }
    /**
     * 更新弃牌堆UI显示
     */
    UpdateDiscardPileUI() {
        const discardButton = this.Widget_CardListButton_0;
        if (discardButton && discardButton.AmountText) {
            discardButton.AmountText.SetText(this.discardPile.Num().toString());
            console.log('[TS_CardMainUI] 更新弃牌堆UI:', this.discardPile.Num());
        }
        else {
            console.warn('[TS_CardMainUI] 未找到Widget_CardListButton_0或其AmountText');
        }
    }
    /**
     * 弃牌（将手牌移动到弃牌堆）
     * @param cardIndex 手牌索引
     */
    DiscardCard(cardIndex) {
        if (cardIndex < 0 || cardIndex >= this.handCards.Num()) {
            console.error(`无效的手牌索引: ${cardIndex}`);
            return;
        }
        // 获取卡牌信息
        const cardInfo = this.handCards.Get(cardIndex);
        // 从手牌中移除
        this.handCards.RemoveAt(cardIndex);
        // 添加到弃牌堆
        this.discardPile.Add(cardInfo);
        // 更新弃牌堆UI
        this.UpdateDiscardPileUI();
        // 移除并回收Widget
        this.RemoveHandCardWidget(cardInfo);
        // 更新布局
        this.UpdateHandCardsLayout();
    }
    /**
     * 通过卡牌信息弃牌
     * @param cardInfo 卡牌信息
     */
    DiscardCardByInfo(cardInfo) {
        // 查找卡牌索引
        for (let i = 0; i < this.handCards.Num(); i++) {
            if (this.handCards.Get(i) === cardInfo) {
                this.DiscardCard(i);
                return;
            }
        }
        console.warn('未找到要弃掉的卡牌');
    }
    /**
     * 通过Widget查找对应的CardInfo
     */
    FindCardInfoByWidget(widget) {
        return this.handCardWidgets.Get(widget);
    }
    /**
     * 通过CardInfo查找对应的Widget
     */
    FindWidgetByCardInfo(cardInfo) {
        // 遍历TMap查找对应的Widget
        for (let i = 0; i < this.handCardWidgets.GetMaxIndex(); i++) {
            if (this.handCardWidgets.IsValidIndex(i)) {
                const widget = this.handCardWidgets.GetKey(i);
                const info = this.handCardWidgets.Get(widget);
                if (info === cardInfo) {
                    return widget;
                }
            }
        }
        return undefined;
    }
    /**
     * 移除手牌Widget并归还到对象池
     */
    RemoveHandCardWidget(cardInfo) {
        if (!this.widgetPool || !this.widgetPool.IsInitialized()) {
            console.warn('RemoveHandCardWidget: Widget Pool未初始化，尝试创建...');
            this.widgetPool = new TS_CardWidgetPool_1.CardWidgetPool(this);
            if (!this.widgetPool || !this.widgetPool.IsInitialized()) {
                console.error('RemoveHandCardWidget: Widget Pool创建失败');
                return;
            }
        }
        // 通过CardInfo查找对应的Widget
        const widget = this.FindWidgetByCardInfo(cardInfo);
        if (widget) {
            // 归还到对象池
            this.widgetPool.Release(widget);
            // 从映射表中移除
            this.handCardWidgets.Remove(widget);
        }
    }
    /**
     * 清空所有手牌
     */
    ClearHand() {
        // 将所有手牌移到弃牌堆
        while (this.handCards.Num() > 0) {
            this.DiscardCard(0);
        }
    }
    /**
     * 洗牌（将弃牌堆放回抽牌堆并洗牌）
     */
    ShuffleDiscardPile() {
        if (this.discardPile.Num() === 0)
            return;
        // 将弃牌堆的牌放回抽牌堆
        for (let i = 0; i < this.discardPile.Num(); i++) {
            this.drawPile.Add(this.discardPile.Get(i));
        }
        // 清空弃牌堆
        this.discardPile.Empty();
        // 洗抽牌堆
        this.ShuffleDrawPile();
        // 更新UI
        this.UpdateDrawPileUI();
        this.UpdateDiscardPileUI();
    }
    /**
     * 洗抽牌堆（Fisher-Yates洗牌算法）
     */
    ShuffleDrawPile() {
        const count = this.drawPile.Num();
        if (count <= 1)
            return;
        // Fisher-Yates洗牌算法
        for (let i = count - 1; i > 0; i--) {
            const j = Math.floor(Math.random() * (i + 1));
            // 交换元素
            const temp = this.drawPile.Get(i);
            this.drawPile.Set(i, this.drawPile.Get(j));
            this.drawPile.Set(j, temp);
        }
    }
    /**
     * 获取当前手牌数量
     */
    GetHandSize() {
        return this.handCards.Num();
    }
    /**
     * 获取Widget Pool状态信息（用于调试）
     */
    GetPoolStats() {
        if (!this.widgetPool)
            return 'Widget Pool未初始化';
        if (!this.widgetPool.IsInitialized())
            return 'Widget Pool初始化失败';
        return `激活: ${this.widgetPool.GetActiveCount()}, 池中: ${this.widgetPool.GetPoolCount()}`;
    }
}
exports.TS_CardMainUI = TS_CardMainUI;
puerts_1.blueprint.mixin(jsClass, TS_CardMainUI);
//# sourceMappingURL=TS_CardMainUI.js.map