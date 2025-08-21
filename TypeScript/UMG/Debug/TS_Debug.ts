import * as UE from 'ue';
import { $ref, blueprint } from 'puerts';

const uclass = UE.Class.Load("/Game/UMG/Debug/W_Debug.W_Debug_C");
const jsClass = blueprint.tojs<typeof UE.Game.UMG.Debug.W_Debug.W_Debug_C>(uclass);

class TS_Debug extends jsClass {

    static readonly EPlayerStateMap: { [key: number]: string } = {
        0: "Idle",
        1: "Walk",
        2: "Run",
        3: "Jump",
        4: "Fall",
        5: "Attack",
        6: "RunAttack",
        7: "Dash"
    };

    static pawn: UE.CurrsorCharacter;
    static PlayerState: UE.CurrsorPlayerState;
    static GameInstance: UE.CurrsorGameInstance;

    Construct() {
        TS_Debug.pawn = this.GetOwningPlayerPawn() as UE.CurrsorCharacter;
        TS_Debug.PlayerState = TS_Debug.pawn.PlayerState as UE.CurrsorPlayerState;
        TS_Debug.GameInstance = this.GetWorld().OwningGameInstance as UE.CurrsorGameInstance;
        this.Debug();
    }
    
    // Debug
    public Debug(): void {
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

    Get_IsDebug_CheckedState(): UE.ECheckBoxState {
        return TS_Debug.GameInstance.bDebug ? UE.ECheckBoxState.Checked : UE.ECheckBoxState.Unchecked;
    }

    Get_State_Text(): string {
        const stateNum = TS_Debug.PlayerState.GetCurrentState();
        return TS_Debug.EPlayerStateMap[stateNum] ?? stateNum.toString();
    }

    BndEvt__W_Debug_IsDebug_K2Node_ComponentBoundEvent_0_OnCheckBoxComponentStateChanged__DelegateSignature(bIsChecked: boolean) : void {
        TS_Debug.GameInstance.bDebug = bIsChecked;
        this.Debug();
    }

}

blueprint.mixin(jsClass, TS_Debug);