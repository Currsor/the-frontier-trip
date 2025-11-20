"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.TS_Card = void 0;
const UE = require("ue");
const puerts_1 = require("puerts");
const uclass = UE.Class.Load("/Game/UI/Blueprints/Cards/Widget_CardCell.Widget_CardCell_C");
const jsClass = puerts_1.blueprint.tojs(uclass);
class TS_Card extends jsClass {
    // 动画系统引用（由外部设置）
    animationSystem = null;
    // 卡牌信息（由外部设置）
    cardInfo = null;
    // 拖动状态
    isDragging = false;
    SetData(cardInfo) {
        this.cardInfo = cardInfo;
        // 设置卡牌名称
        if (this.bp_CardName) {
            this.bp_CardName.SetText(cardInfo.Name);
        }
        // 设置法力消耗
        if (this.bp_ManaCost) {
            this.bp_ManaCost.SetText(cardInfo.Consumption.toString());
        }
        // 设置描述
        if (this.bp_Description) {
            this.bp_Description.SetText(cardInfo.Description);
        }
        // 设置类型
        if (this.bp_Type) {
            this.bp_Type.SetText(cardInfo.Type);
        }
    }
    /**
     * 鼠标进入事件
     */
    OnMouseEnter(MyGeometry, MouseEvent) {
        if (this.animationSystem) {
            this.animationSystem.StartHoverAnimation(this);
        }
    }
    /**
     * 鼠标离开事件
     */
    OnMouseLeave(MouseEvent) {
        if (this.animationSystem) {
            this.animationSystem.EndHoverAnimation(this);
        }
    }
    /**
     * 拖动检测事件
     */
    OnDragDetected(MyGeometry, PointerEvent, Operation) {
        console.log('[TS_Card.OnDragDetected] 拖动检测事件触发');
        this.isDragging = true;
        const dragDropOp = UE.WidgetBlueprintLibrary.CreateDragDropOperation(UE.DragDropOperation.StaticClass());
        // 创建拖动视觉Widget实例
        const dragWidgetClass = UE.Class.Load("/Game/UI/Blueprints/Cards/Widget_CardCell.Widget_CardCell_C");
        const owningPlayer = this.GetOwningPlayer();
        const dragWidget = UE.WidgetBlueprintLibrary.Create(this, dragWidgetClass, owningPlayer);
        // 设置拖动Widget的卡牌信息
        if (this.cardInfo) {
            dragWidget.SetData(this.cardInfo);
        }
        // 设置拖动视觉效果
        dragDropOp.DefaultDragVisual = dragWidget;
        dragDropOp.Pivot = UE.EDragPivot.CenterCenter;
        dragDropOp.Payload = this;
        // if (this.animationSystem) {
        //     console.log('[TS_Card.OnDragDetected] 调用animationSystem.StartDragCard');
        //     this.animationSystem.StartDragCard(this);
        // }
        // 
        // if (this.mainUI) {
        //     console.log('[TS_Card.OnDragDetected] 调用mainUI.OnCardDragStart');
        //     this.mainUI.OnCardDragStart(this);
        // }
        (0, puerts_1.$set)(Operation, dragDropOp);
    }
}
exports.TS_Card = TS_Card;
puerts_1.blueprint.mixin(jsClass, TS_Card);
//# sourceMappingURL=TS_Card.js.map