"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const UE = require("ue");
const puerts_1 = require("puerts");
const uclass = UE.Class.Load("/Game/UI/Blueprints/Cards/Panel_Card.Panel_Card_C");
const jsClass = puerts_1.blueprint.tojs(uclass);
class TS_Card extends jsClass {
    BndEvt__Panel_Card_Esc_K2Node_ComponentBoundEvent_0_OnButtonClickedEvent__DelegateSignature() {
        this.RemoveFromParent();
    }
}
puerts_1.blueprint.mixin(jsClass, TS_Card);
//# sourceMappingURL=TS_Panel_Card.js.map