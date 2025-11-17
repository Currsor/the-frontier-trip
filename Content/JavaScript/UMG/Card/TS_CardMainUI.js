"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.TS_CardMainUI = void 0;
const UE = require("ue");
const puerts_1 = require("puerts");
const TS_CardWidgetPool_1 = require("./TS_CardWidgetPool");
const GameConfig_1 = require("../../Config/GameConfig");
const uclass = UE.Class.Load("/Game/UI/Blueprints/Cards/Widget_CardMainUI.Widget_CardMainUI_C");
const jsClass = puerts_1.blueprint.tojs(uclass);
class TS_CardMainUI {
    static pawn;
    static PlayerState;
    // Widget Pool管理器
    widgetPool = null;
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
        this.InitCard();
    }
    Destruct() {
        // 清理Widget Pool
        if (this.widgetPool) {
            this.widgetPool.Clear();
            this.widgetPool = null;
        }
        this.handCardWidgets.Empty();
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
    }
    // 抽取手牌
    AddCard(numCards = 1) {
        if (!this.widgetPool || !this.widgetPool.IsInitialized()) {
            console.error('AddCard: Widget Pool未初始化');
            return;
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
        this.UpdateHandCardsLayout();
    }
    /**
     * 创建手牌Widget并添加到UI
     */
    CreateHandCardWidget(cardInfo) {
        if (!this.widgetPool || !this.widgetPool.IsInitialized()) {
            console.error('CreateHandCardWidget: Widget Pool未初始化');
            return;
        }
        // 从对象池获取Widget
        const cardWidget = this.widgetPool.Acquire();
        if (!cardWidget) {
            console.error('无法从Widget Pool获取卡牌Widget');
            return;
        }
        // 设置卡牌数据
        this.SetupCardWidget(cardWidget, cardInfo);
        // 添加到父容器（这里需要根据实际的UI结构调整）
        // 假设有一个HandCardsContainer来容纳手牌
        // this.HandCardsContainer.AddChild(cardWidget);
        cardWidget.AddToViewport(0);
        // 记录映射关系（键为Widget，值为CardInfo）
        this.handCardWidgets.Add(cardWidget, cardInfo);
    }
    /**
     * 设置卡牌Widget的数据
     */
    SetupCardWidget(widget, cardInfo) {
        // 设置卡牌名称
        if (widget.bp_CardName) {
            widget.bp_CardName.SetText(cardInfo.Name);
        }
        // 设置法力消耗
        if (widget.bp_ManaCost) {
            widget.bp_ManaCost.SetText(cardInfo.Consumption.toString());
        }
        // 设置描述
        if (widget.bp_Description) {
            widget.bp_Description.SetText(cardInfo.Description);
        }
        // 设置类型
        if (widget.bp_Type) {
            widget.bp_Type.SetText(cardInfo.Type);
        }
        // 可以在这里设置更多的卡牌数据，如图片、稀有度等
    }
    /**
     * 更新手牌布局
     */
    UpdateHandCardsLayout() {
        // 这里实现手牌的排列逻辑
        // 可以根据手牌数量动态调整位置和间距
        const handSize = this.handCards.Num();
        const spacing = GameConfig_1.GameConfig.CARD_CONFIG.CARD_SPACING;
        // TODO: 实现具体的布局逻辑
        // 例如：将卡牌排列成弧形，居中显示等
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
            console.warn('RemoveHandCardWidget: Widget Pool未初始化');
            return;
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