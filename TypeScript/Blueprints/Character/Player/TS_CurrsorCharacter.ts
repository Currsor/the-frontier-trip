import * as UE from 'ue';
import { $ref, blueprint } from 'puerts';

const uclass = UE.Class.Load("/Game/Blueprints/Character/Player/BP_CurrsorCharacter.BP_CurrsorCharacter_C");
const jsClass = blueprint.tojs<typeof UE.Game.Blueprints.Character.Player.BP_CurrsorCharacter.BP_CurrsorCharacter_C>(uclass);

class TS_CurrsorCharacter extends jsClass {

}

blueprint.mixin(jsClass, TS_CurrsorCharacter);