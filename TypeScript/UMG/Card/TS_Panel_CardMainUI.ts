import * as UE from 'ue';
import { $ref, blueprint } from 'puerts';

import { TS_CardMainUI } from './TS_CardMainUI';

const uclass = UE.Class.Load("/Game/UI/Blueprints/Cards/Panel_CardMainUI.Panel_CardMainUI_C");
const jsClass = blueprint.tojs(uclass);

interface TS_Panel_CardMainUI extends UE.Game.UI.Blueprints.Cards.Panel_CardMainUI.Panel_CardMainUI_C {}

class TS_Panel_CardMainUI extends jsClass {
    declare Widget_CardMainUI: TS_CardMainUI;

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
    private OnAddButtonClicked(): void {
        this.AddCard();
        this.LogPoolStats();
    }

    /**
     * 添加一张卡牌到手牌
     */
    private AddCard(): void {
        this.Widget_CardMainUI.AddCard();
    }
    
    /**
     * 添加多张卡牌到手牌
     * @param count 卡牌数量
     */
    public AddCards(count: number): void {
        this.Widget_CardMainUI.AddCard(count);
        this.LogPoolStats();
    }
    
    /**
     * 弃掉第一张手牌
     */
    public DiscardFirstCard(): void {
        if (this.Widget_CardMainUI.GetHandSize() > 0) {
            this.Widget_CardMainUI.DiscardCard(0);
            this.LogPoolStats();
        } else {
            console.log('手牌为空，无法弃牌');
        }
    }
    
    /**
     * 清空所有手牌
     */
    public ClearAllCards(): void {
        this.Widget_CardMainUI.ClearHand();
        console.log('已清空所有手牌');
        this.LogPoolStats();
    }
    
    /**
     * 洗牌（将弃牌堆洗回抽牌堆）
     */
    public ShuffleCards(): void {
        this.Widget_CardMainUI.ShuffleDiscardPile();
        console.log('已将弃牌堆洗回抽牌堆');
    }
    
    /**
     * 打印Widget Pool状态（用于调试）
     */
    private LogPoolStats(): void {
        const handSize = this.Widget_CardMainUI.GetHandSize();
        const poolStats = this.Widget_CardMainUI.GetPoolStats();
        console.log(`手牌数: ${handSize}, Widget Pool状态: ${poolStats}`);
    }
}

blueprint.mixin(jsClass, TS_Panel_CardMainUI);