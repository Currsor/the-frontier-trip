import * as UE from 'ue';
import { $ref, blueprint } from 'puerts';

const uclass = UE.Class.Load("/Game/UI/Blueprints/Cards/Panel_CardMainUI.Panel_CardMainUI_C");
const jsClass = blueprint.tojs<typeof UE.Game.UI.Blueprints.Cards.Panel_CardMainUI.Panel_CardMainUI_C>(uclass);

interface TS_Panel_CardMainUI extends UE.Game.UI.Blueprints.Cards.Panel_CardMainUI.Panel_CardMainUI_C {}

class TS_Panel_CardMainUI extends jsClass {
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
        // Add card logic here
        

    }
}

blueprint.mixin(jsClass, TS_Panel_CardMainUI);