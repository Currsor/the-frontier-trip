import * as UE from 'ue';
import { $ref, blueprint } from 'puerts';

const uclass = UE.Class.Load("/Game/UI/Blueprints/Cards/Panel_Card.Panel_Card_C");
const jsClass = blueprint.tojs<typeof UE.Game.UI.Blueprints.Cards.Panel_Card.Panel_Card_C>(uclass);

interface TS_Card extends UE.Game.UI.Blueprints.Cards.Panel_Card.Panel_Card_C {}

class TS_Card extends jsClass {
    BndEvt__Panel_Card_Esc_K2Node_ComponentBoundEvent_0_OnButtonClickedEvent__DelegateSignature(): void {
        this.RemoveFromParent();
    }
    
}

blueprint.mixin(jsClass, TS_Card);