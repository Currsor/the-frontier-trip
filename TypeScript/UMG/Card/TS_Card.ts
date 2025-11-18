import * as UE from 'ue';
import { $ref, blueprint } from 'puerts';

const uclass = UE.Class.Load("/Game/UI/Blueprints/Cards/Widget_CardCell.Widget_CardCell_C");
const jsClass = blueprint.tojs(uclass);

interface TS_Card extends UE.Game.UI.Blueprints.Cards.Widget_CardCell.Widget_CardCell_C {}

class TS_Card extends jsClass {
    
}

blueprint.mixin(jsClass, TS_Card);