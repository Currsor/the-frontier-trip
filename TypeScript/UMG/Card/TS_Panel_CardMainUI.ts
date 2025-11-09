import * as UE from 'ue';
import { $ref, blueprint } from 'puerts';

import { TS_CardMainUI } from './TS_CardMainUI';

const uclass = UE.Class.Load("/Game/UI/Blueprints/Cards/Panel_CardMainUI.Panel_CardMainUI_C");
const jsClass = blueprint.tojs(uclass);

interface TS_Panel_CardMainUI extends UE.Game.UI.Blueprints.Cards.Panel_CardMainUI.Panel_CardMainUI_C {}

class TS_Panel_CardMainUI extends jsClass {
    declare Widget_CardMainUI: TS_CardMainUI;

    Construct() {
        if (this.bp_test_add && this.bp_test_add.bp_btn) {
            this.bp_test_add.bp_btn.OnClicked.Add(() => {
                this.OnAddButtonClicked();
            });
        }
    }

    private OnAddButtonClicked(): void {
        this.AddCard();
    }

    private AddCard(): void {
        this.Widget_CardMainUI.AddCard();
    }
}

blueprint.mixin(jsClass, TS_Panel_CardMainUI);