"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const UE = require("ue");
const puerts_1 = require("puerts");
const uclass = UE.Class.Load("/Game/UI/Blueprints/Cards/Panel_CardMainUI.Panel_CardMainUI_C");
const jsClass = puerts_1.blueprint.tojs(uclass);
class TS_Panel_CardMainUI extends jsClass {
    Construct() {
        // 绑定添加卡牌按钮
        if (this.bp_test_add && this.bp_test_add.bp_btn) {
            this.bp_test_add.bp_btn.OnClicked.Add(() => {
                this.OnAddButtonClicked();
            });
        }
        // 可以在这里添加更多测试按钮的绑定
        // 例如：弃牌按钮、清空手牌按钮、洗牌按钮等
    }
    /**
     * 添加卡牌按钮点击事件
     */
    OnAddButtonClicked() {
        this.AddCard();
        this.LogPoolStats();
    }
    /**
     * 添加一张卡牌到手牌
     */
    AddCard() {
        this.Widget_CardMainUI.AddCard();
    }
    /**
     * 添加多张卡牌到手牌
     * @param count 卡牌数量
     */
    AddCards(count) {
        this.Widget_CardMainUI.AddCard(count);
        this.LogPoolStats();
    }
    /**
     * 弃掉第一张手牌
     */
    DiscardFirstCard() {
        if (this.Widget_CardMainUI.GetHandSize() > 0) {
            this.Widget_CardMainUI.DiscardCard(0);
            this.LogPoolStats();
        }
        else {
            console.log('手牌为空，无法弃牌');
        }
    }
    /**
     * 清空所有手牌
     */
    ClearAllCards() {
        this.Widget_CardMainUI.ClearHand();
        console.log('已清空所有手牌');
        this.LogPoolStats();
    }
    /**
     * 洗牌（将弃牌堆洗回抽牌堆）
     */
    ShuffleCards() {
        this.Widget_CardMainUI.ShuffleDiscardPile();
        console.log('已将弃牌堆洗回抽牌堆');
    }
    /**
     * 打印Widget Pool状态（用于调试）
     */
    LogPoolStats() {
        const handSize = this.Widget_CardMainUI.GetHandSize();
        const poolStats = this.Widget_CardMainUI.GetPoolStats();
        console.log(`手牌数: ${handSize}, Widget Pool状态: ${poolStats}`);
    }
}
puerts_1.blueprint.mixin(jsClass, TS_Panel_CardMainUI);
//# sourceMappingURL=TS_Panel_CardMainUI.js.map