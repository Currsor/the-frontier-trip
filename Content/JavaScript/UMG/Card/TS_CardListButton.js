"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const UE = require("ue");
const puerts_1 = require("puerts");
const uclass = UE.Class.Load("/Game/UI/Blueprints/Cards/Widget_CardListButton.Widget_CardListButton_C");
const jsClass = puerts_1.blueprint.tojs(uclass);
class TS_CardListButton extends jsClass {
}
puerts_1.blueprint.mixin(jsClass, TS_CardListButton);
//# sourceMappingURL=TS_CardListButton.js.map