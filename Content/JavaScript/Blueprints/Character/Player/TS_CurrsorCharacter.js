"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const UE = require("ue");
const puerts_1 = require("puerts");
const uclass = UE.Class.Load("/Game/Blueprints/Character/Player/BP_CurrsorCharacter.BP_CurrsorCharacter_C");
const jsClass = puerts_1.blueprint.tojs(uclass);
class TS_CurrsorCharacter extends jsClass {
    State = UE.EPlayerState.Idle;
    ReceiveTick() {
        this.Debug();
    }
    // Debug
    Debug() {
        if (this.GetWorld().OwningGameInstance.bIsDebug) {
            this.ArrowComponent_EditorOnly.SetHiddenInGame(false, true);
            this.ArrowComponent_EditorOnly.SetIsScreenSizeScaled(true);
            this.State = this.PlayerState.GetCurrentState();
            UE.KismetSystemLibrary.PrintString(this, "PlayerState: " + UE.EPlayerState[this.State], true, false, UE.LinearColor.White, 0.0);
        }
    }
}
puerts_1.blueprint.mixin(jsClass, TS_CurrsorCharacter);
//# sourceMappingURL=TS_CurrsorCharacter.js.map