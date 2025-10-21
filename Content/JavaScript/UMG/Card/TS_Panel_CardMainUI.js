"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const UE = require("ue");
const puerts_1 = require("puerts");
const uclass = UE.Class.Load("/Game/UI/Blueprints/Cards/Panel_CardMainUI.Panel_CardMainUI_C");
const jsClass = puerts_1.blueprint.tojs(uclass);
class TS_Panel_CardMainUI extends jsClass {
    Construct() {
        if (this.bp_test_add && this.bp_test_add.bp_btn) {
            this.bp_test_add.bp_btn.OnClicked.Add(() => {
                this.OnAddButtonClicked();
            });
        }
    }
    OnAddButtonClicked() {
        this.AddCard();
    }
    AddCard() {
        // Add card logic here
    }
}
puerts_1.blueprint.mixin(jsClass, TS_Panel_CardMainUI);
//# sourceMappingURL=TS_Panel_CardMainUI.js.map