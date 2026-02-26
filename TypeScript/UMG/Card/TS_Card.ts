import * as UE from 'ue';
import { $ref, $Ref, $set, $Nullable, blueprint } from 'puerts';
import { CardAnimationSystem } from './TS_CardAnimationSystem';
import { EventSystem } from '../../Systems/EventSystem';

const uclass = UE.Class.Load("/Game/UI/Blueprints/Cards/Widget_CardCell.Widget_CardCell_C");
const jsClass = blueprint.tojs(uclass);

export interface TS_Card extends UE.Game.UI.Blueprints.Cards.Widget_CardCell.Widget_CardCell_C {}

export class TS_Card extends jsClass {
    // 动画系统引用（由外部设置）
    public animationSystem: CardAnimationSystem | null = null;

    // 卡牌信息（由外部设置）
    public cardInfo: UE.Game.Data.Structs.S_CardInfo.S_CardInfo | null = null;
    
    // 拖动状态
    public isDragging: boolean = false;
    
    // 拖动前的原始状态（用于还原）
    public originalPosition: UE.Vector2D | null = null;
    public originalRotation: number = 0;
    public originalScale: UE.Vector2D | null = null;
    public originalZOrder: number = 0;

    public SetData(cardInfo: UE.Game.Data.Structs.S_CardInfo.S_CardInfo): void {
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
    OnMouseEnter(MyGeometry: UE.Geometry, MouseEvent: UE.PointerEvent): void {
        if (this.animationSystem) {
            this.animationSystem.StartHoverAnimation(this);
        }
    }
    
    /**
     * 鼠标离开事件
     */
    OnMouseLeave(MouseEvent: UE.PointerEvent): void {
        if (this.animationSystem) {
            this.animationSystem.EndHoverAnimation(this);
        }
    }

    /**
     * 拖动检测事件
     */
    OnDragDetected(MyGeometry: UE.Geometry, PointerEvent: UE.PointerEvent, Operation: $Ref<UE.DragDropOperation>): void {
        console.log('[TS_Card.OnDragDetected] 拖动检测事件触发');
        this.isDragging = true;
        
        // 通知主UI卡牌开始拖动
        EventSystem.emit("CardDragStart", { card: this });
        
        const dragDropOp = UE.WidgetBlueprintLibrary.CreateDragDropOperation(UE.DragDropOperation.StaticClass());
        
        // 创建拖动视觉Widget实例
        const dragWidgetClass = UE.Class.Load("/Game/UI/Blueprints/Cards/Widget_CardCell.Widget_CardCell_C");
        const owningPlayer = this.GetOwningPlayer();
        const dragWidget = UE.WidgetBlueprintLibrary.Create(this, dragWidgetClass, owningPlayer) as TS_Card;
        
        // 设置拖动Widget的卡牌信息
        if (this.cardInfo) {
            dragWidget.SetData(this.cardInfo);
        }
        
        // 设置拖动视觉效果
        dragDropOp.DefaultDragVisual = dragWidget;
        dragDropOp.Pivot = UE.EDragPivot.CenterCenter;
        dragDropOp.Payload = this;
        
        $set(Operation, dragDropOp);
    }
    
    /**
     * 拖动取消事件（用户拖动后没有放到有效区域）
     */
    OnDragCancelled(PointerEvent: UE.PointerEvent, Operation: $Nullable<UE.DragDropOperation>): void {
        console.log('[TS_Card.OnDragCancelled] 拖动取消');
        this.isDragging = false;
        
        // 通知主UI拖动失败
        EventSystem.emit("CardDragEnd", { card: this, success: false });
    }
}

blueprint.mixin(jsClass, TS_Card);