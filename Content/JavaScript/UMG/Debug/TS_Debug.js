"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const UE = require("ue");
const puerts_1 = require("puerts");
const uclass = UE.Class.Load("/Game/UMG/Debug/W_Debug.W_Debug_C");
const jsClass = puerts_1.blueprint.tojs(uclass);
class TS_Debug extends jsClass {
    static EPlayerStateMap = {
        0: "Idle",
        1: "Walk",
        2: "Run",
        3: "Jump",
        4: "Fall",
        5: "Attack",
        6: "RunAttack",
        7: "Dash"
    };
    static pawn;
    static PlayerState;
    static GameInstance;
    Construct() {
        TS_Debug.pawn = this.GetOwningPlayerPawn();
        TS_Debug.PlayerState = TS_Debug.pawn.PlayerState;
        TS_Debug.GameInstance = this.GetWorld().OwningGameInstance;
        this.Debug();
    }
    // Debug
    Debug() {
        if (TS_Debug.GameInstance.bDebug) {
            TS_Debug.pawn.ArrowComponent_EditorOnly.SetHiddenInGame(false, true);
            TS_Debug.pawn.ArrowComponent_EditorOnly.SetIsScreenSizeScaled(true);
            this.Get_State_Text();
            // UE.KismetSystemLibrary.PrintString(this, "PlayerState: " + UE.EPlayerState[this.TS_State], true, false, UE.LinearColor.White, 0.0);
            if (TS_Debug.GameInstance.bAttackDebug) {
                TS_Debug.pawn.AttackHitbox.SetHiddenInGame(false, true);
            }
        }
        else {
            TS_Debug.pawn.ArrowComponent_EditorOnly.SetHiddenInGame(true, true);
            TS_Debug.pawn.ArrowComponent_EditorOnly.SetIsScreenSizeScaled(false);
            TS_Debug.pawn.AttackHitbox.SetHiddenInGame(true, true);
        }
    }
    Get_IsDebug_CheckedState() {
        return TS_Debug.GameInstance.bDebug ? UE.ECheckBoxState.Checked : UE.ECheckBoxState.Unchecked;
    }
    Get_State_Text() {
        const stateNum = TS_Debug.PlayerState.GetCurrentState();
        return TS_Debug.EPlayerStateMap[stateNum] ?? stateNum.toString();
    }
    BndEvt__W_Debug_IsDebug_K2Node_ComponentBoundEvent_0_OnCheckBoxComponentStateChanged__DelegateSignature(bIsChecked) {
        TS_Debug.GameInstance.bDebug = bIsChecked;
        this.Debug();
    }
}
puerts_1.blueprint.mixin(jsClass, TS_Debug);
//# sourceMappingURL=TS_Debug.js.map