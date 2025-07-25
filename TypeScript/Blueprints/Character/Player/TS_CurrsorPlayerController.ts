import * as UE from 'ue';
import { $ref, blueprint } from 'puerts';

const uclass = UE.Class.Load("/Game/Blueprints/Character/Player/BP_CurrsorPlayerController.BP_CurrsorPlayerController_C");

const jsClass = blueprint.tojs<typeof UE.Game.Blueprints.Character.Player.BP_CurrsorPlayerController.BP_CurrsorPlayerController_C>(uclass);

class TS_CurrsorPlayerController extends jsClass {

}

blueprint.mixin(jsClass, TS_CurrsorPlayerController);