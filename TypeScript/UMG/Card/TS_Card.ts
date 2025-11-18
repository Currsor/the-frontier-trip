import * as UE from 'ue';
import { $ref, blueprint } from 'puerts';
import { CardAnimationSystem } from './TS_CardAnimationSystem';

const uclass = UE.Class.Load("/Game/UI/Blueprints/Cards/Widget_CardCell.Widget_CardCell_C");
const jsClass = blueprint.tojs(uclass);

interface TS_Card extends UE.Game.UI.Blueprints.Cards.Widget_CardCell.Widget_CardCell_C {}

class TS_Card extends jsClass {
    // 动画系统引用（由外部设置）
    public animationSystem: CardAnimationSystem | null = null;
    
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
}

blueprint.mixin(jsClass, TS_Card);