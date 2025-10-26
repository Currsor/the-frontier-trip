import * as UE from 'ue';
import { $ref, blueprint } from 'puerts';

const uclass = UE.Class.Load("/Game/UI/Blueprints/Cards/Widget_CardListButton.Widget_CardListButton_C");
const jsClass = blueprint.tojs<typeof UE.Game.UI.Blueprints.Cards.Widget_CardListButton.Widget_CardListButton_C>(uclass);

interface TS_CardListButton extends UE.Game.UI.Blueprints.Cards.Widget_CardListButton.Widget_CardListButton_C {}

class TS_CardListButton extends jsClass {
}

blueprint.mixin(jsClass, TS_CardListButton);