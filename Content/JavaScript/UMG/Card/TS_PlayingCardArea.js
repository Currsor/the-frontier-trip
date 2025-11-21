"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.TS_PlayingCardArea = void 0;
const UE = require("ue");
const puerts_1 = require("puerts");
const EventSystem_1 = require("../../Systems/EventSystem");
const uclass = UE.Class.Load("/Game/UI/Blueprints/Cards/Widget_PlayingCardArea.Widget_PlayingCardArea_C");
const jsClass = puerts_1.blueprint.tojs(uclass);
class TS_PlayingCardArea extends jsClass {
    OnDragEnter(MyGeometry, PointerEvent, Operation) {
        console.log("[PlayingCardArea] OnDragEnter 被触发");
        if (Operation && Operation.Payload) {
            const card = Operation.Payload;
            if (card && card.cardInfo) {
                console.log(`[PlayingCardArea] 卡牌类型: ${card.cardInfo.Type}`);
            }
        }
    }
    OnDrop(MyGeometry, PointerEvent, Operation) {
        console.log("[PlayingCardArea] OnDrop 被触发");
        if (!Operation || !Operation.Payload) {
            console.log("[PlayingCardArea] OnDrop: Operation 或 Payload 为空");
            return false;
        }
        // 从 Operation 的 Payload 中获取 TS_Card 实例
        const card = Operation.Payload;
        if (card && card.cardInfo) {
            EventSystem_1.EventSystem.emit("Consumption", {
                cardInfo: card.cardInfo,
                target: "Player"
            });
        }
        console.log("[PlayingCardArea] OnDrop 返回 true");
        return true;
    }
}
exports.TS_PlayingCardArea = TS_PlayingCardArea;
puerts_1.blueprint.mixin(jsClass, TS_PlayingCardArea);
//# sourceMappingURL=TS_PlayingCardArea.js.map