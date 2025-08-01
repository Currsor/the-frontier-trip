"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const UE = require("ue");
const puerts_1 = require("puerts");
const uclass = UE.Class.Load("/Game/UMG/Debug/W_Debug.W_Debug_C");
const jsClass = puerts_1.blueprint.tojs(uclass);
class TS_Debug extends jsClass {
    Tick(MyGeometry, InDeltaTime) {
        this.Debug();
    }
    // Debug
    Debug() {
        if (this.GetWorld().OwningGameInstance.bDebug) {
            const pawn = this.GetOwningPlayerPawn();
            pawn.ArrowComponent_EditorOnly.SetHiddenInGame(false, true);
            pawn.ArrowComponent_EditorOnly.SetIsScreenSizeScaled(true);
            this.State.SetText(UE.EPlayerState[pawn.PlayerState.GetCurrentState()]);
            // UE.KismetSystemLibrary.PrintString(this, "PlayerState: " + UE.EPlayerState[this.TS_State], true, false, UE.LinearColor.White, 0.0);
            if (this.GetWorld().OwningGameInstance.bAttackDebug) {
                pawn.AttackHitbox.SetHiddenInGame(false, true);
                pawn.AttackHitbox.SetIsScreenSizeScaled(true);
            }
        }
    }
    Get_IsDebug_CheckedState() {
        return this.GetWorld().OwningGameInstance.bDebug ? UE.ECheckBoxState.Checked : UE.ECheckBoxState.Unchecked;
    }
    BndEvt__W_Debug_IsDebug_K2Node_ComponentBoundEvent_0_OnCheckBoxComponentStateChanged__DelegateSignature(bIsChecked) {
        this.GetWorld().OwningGameInstance.bDebug = bIsChecked;
    }
}
puerts_1.blueprint.mixin(jsClass, TS_Debug);
//# sourceMappingURL=TS_Debug.js.map