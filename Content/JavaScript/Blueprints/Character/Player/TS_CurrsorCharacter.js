"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const UE = require("ue");
const puerts_1 = require("puerts");
const uclass = UE.Class.Load("/Game/Blueprints/Character/Player/BP_CurrsorCharacter.BP_CurrsorCharacter_C");
const jsClass = puerts_1.blueprint.tojs(uclass);
class TS_CurrsorCharacter extends jsClass {
}
puerts_1.blueprint.mixin(jsClass, TS_CurrsorCharacter);
//# sourceMappingURL=TS_CurrsorCharacter.js.map