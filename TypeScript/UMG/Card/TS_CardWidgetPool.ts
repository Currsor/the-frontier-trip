import * as UE from 'ue';
import { GameConfig } from '../../Config/GameConfig';

/**
 * 卡牌Widget对象池
 * 用于管理卡牌Widget的创建、回收和复用，提高性能
 */
export class CardWidgetPool {
    private pool: UE.Game.UI.Blueprints.Cards.Widget_CardCell.Widget_CardCell_C[] = [];
    private activeWidgets: Set<UE.Game.UI.Blueprints.Cards.Widget_CardCell.Widget_CardCell_C> = new Set();
    private widgetClass: UE.Class | null = null;
    private owningWidget: UE.UserWidget;
    private isInitialized: boolean = false;

    constructor(owningWidget: UE.UserWidget) {
        this.owningWidget = owningWidget;
        // 延迟初始化，避免在构造函数中立即加载
        this.Initialize();
    }
    
    /**
     * 初始化对象池
     */
    private Initialize(): void {
        try {
            // 加载卡牌Widget类
            this.widgetClass = UE.Class.Load("/Game/UI/Blueprints/Cards/Widget_CardCell.Widget_CardCell_C");
            
            if (!this.widgetClass) {
                console.error('CardWidgetPool: 无法加载Widget类');
                return;
            }
            
            // 预创建Widget池
            this.PrewarmPool();
            this.isInitialized = true;
            
            console.log(`CardWidgetPool: 初始化成功，池大小: ${this.pool.length}`);
        } catch (error) {
            console.error('CardWidgetPool: 初始化失败', error);
        }
    }

    /**
     * 预热对象池，提前创建一定数量的Widget
     */
    private PrewarmPool(): void {
        const poolSize = GameConfig.CARD_CONFIG.WIDGET_POOL_SIZE;
        for (let i = 0; i < poolSize; i++) {
            const widget = this.CreateNewWidget();
            if (widget) {
                this.pool.push(widget);
            }
        }
    }

    /**
     * 创建新的Widget实例
     */
    private CreateNewWidget(): UE.Game.UI.Blueprints.Cards.Widget_CardCell.Widget_CardCell_C | null {
        if (!this.widgetClass) {
            console.error('CardWidgetPool: Widget类未加载');
            return null;
        }
        
        try {
            const widget = UE.WidgetBlueprintLibrary.Create(
                this.owningWidget,
                this.widgetClass,
                this.owningWidget.GetOwningPlayer()
            ) as UE.Game.UI.Blueprints.Cards.Widget_CardCell.Widget_CardCell_C;
            
            return widget || null;
        } catch (error) {
            console.error('CardWidgetPool: 创建Widget失败', error);
            return null;
        }
    }

    /**
     * 从对象池获取一个Widget
     * @returns 卡牌Widget实例
     */
    public Acquire(): UE.Game.UI.Blueprints.Cards.Widget_CardCell.Widget_CardCell_C | null {
        if (!this.isInitialized) {
            console.warn('CardWidgetPool: 对象池未初始化，尝试重新初始化');
            this.Initialize();
            if (!this.isInitialized) {
                console.error('CardWidgetPool: 重新初始化失败');
                return null;
            }
        }
        
        let widget: UE.Game.UI.Blueprints.Cards.Widget_CardCell.Widget_CardCell_C | null = null;

        // 从池中获取
        if (this.pool.length > 0) {
            widget = this.pool.pop()!;
        } else {
            // 池为空，创建新的
            widget = this.CreateNewWidget();
        }

        if (widget) {
            this.activeWidgets.add(widget);
            // 重置Widget状态
            this.ResetWidget(widget);
        }

        return widget;
    }

    /**
     * 将Widget归还到对象池
     * @param widget 要归还的Widget
     */
    public Release(widget: UE.Game.UI.Blueprints.Cards.Widget_CardCell.Widget_CardCell_C): void {
        if (!widget) return;

        // 从激活列表中移除
        if (this.activeWidgets.has(widget)) {
            this.activeWidgets.delete(widget);
        }

        // 从父级移除
        widget.RemoveFromParent();

        // 归还到池中
        if (this.pool.length < GameConfig.CARD_CONFIG.WIDGET_POOL_SIZE * 2) {
            this.pool.push(widget);
        }
    }

    /**
     * 重置Widget状态
     * @param widget 要重置的Widget
     */
    private ResetWidget(widget: UE.Game.UI.Blueprints.Cards.Widget_CardCell.Widget_CardCell_C): void {
        // 重置可见性
        widget.SetVisibility(UE.ESlateVisibility.Visible);
        
        // 重置Transform
        widget.SetRenderScale(new UE.Vector2D(1, 1));
        widget.SetRenderOpacity(1.0);
    }

    /**
     * 释放所有Widget
     */
    public ReleaseAll(): void {
        // 释放所有激活的Widget
        const activeArray = Array.from(this.activeWidgets);
        for (const widget of activeArray) {
            this.Release(widget);
        }
    }

    /**
     * 清空对象池
     */
    public Clear(): void {
        this.ReleaseAll();
        this.pool = [];
        this.activeWidgets.clear();
    }

    /**
     * 获取激活的Widget数量
     */
    public GetActiveCount(): number {
        return this.activeWidgets.size;
    }

    /**
     * 获取池中可用的Widget数量
     */
    public GetPoolCount(): number {
        return this.pool.length;
    }
    
    /**
     * 检查对象池是否已初始化
     */
    public IsInitialized(): boolean {
        return this.isInitialized;
    }
}
