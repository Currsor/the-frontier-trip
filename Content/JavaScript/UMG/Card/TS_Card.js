"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const UE = require("ue");
const puerts_1 = require("puerts");
const uclass = UE.Class.Load("/Game/UI/Blueprints/Cards/Widget_CardCell.Widget_CardCell_C");
const jsClass = puerts_1.blueprint.tojs(uclass);
class TS_Card extends jsClass {
    // 动画系统引用（由外部设置）
    animationSystem = null;
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
}
puerts_1.blueprint.mixin(jsClass, TS_Card);
//# sourceMappingURL=TS_Card.js.map