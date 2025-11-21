import * as UE from 'ue';
import { $ref, $Ref, $set, $Nullable, blueprint } from 'puerts';
import { EventSystem } from '../../Systems/EventSystem';
import { TS_Card } from './TS_Card';

const uclass = UE.Class.Load("/Game/UI/Blueprints/Cards/Widget_PlayingCardArea.Widget_PlayingCardArea_C");
const jsClass = blueprint.tojs(uclass);

export interface TS_PlayingCardArea extends UE.Game.UI.Blueprints.Cards.Widget_PlayingCardArea.Widget_PlayingCardArea_C {}

export class TS_PlayingCardArea extends jsClass {
    OnDragEnter(MyGeometry: UE.Geometry, PointerEvent: UE.PointerEvent, Operation: $Nullable<UE.DragDropOperation>) : void{
        console.log("[PlayingCardArea] OnDragEnter 被触发");
        
        if (Operation && Operation.Payload) {
            const card = Operation.Payload as TS_Card;
            if (card && card.cardInfo) {
                console.log(`[PlayingCardArea] 卡牌类型: ${card.cardInfo.Type}`);
            }
        }
    }

    OnDrop(MyGeometry: UE.Geometry, PointerEvent: UE.PointerEvent, Operation: $Nullable<UE.DragDropOperation>) : boolean{
        console.log("[PlayingCardArea] OnDrop 被触发");
        
        if (!Operation || !Operation.Payload) {
            console.log("[PlayingCardArea] OnDrop: Operation 或 Payload 为空");
            return false;
        }

        // 从 Operation 的 Payload 中获取 TS_Card 实例
        const card = Operation.Payload as TS_Card;
            
        if (card && card.cardInfo) {
            EventSystem.emit("Consumption", {
                cardInfo: card.cardInfo,
                target: "Player"
            });
        }
        
        console.log("[PlayingCardArea] OnDrop 返回 true");
        return true;
    }
}

blueprint.mixin(jsClass, TS_PlayingCardArea);