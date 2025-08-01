import * as UE from 'ue';
import { $ref, blueprint } from 'puerts';

const uclass = UE.Class.Load("/Game/UMG/Debug/W_Debug.W_Debug_C");
const jsClass = blueprint.tojs<typeof UE.Game.UMG.Debug.W_Debug.W_Debug_C>(uclass);

class TS_Debug extends jsClass {

    OnInitialized(): void {
        this.Debug();
    }
    
    // Debug
    public Debug(): void {
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
        else {
            const pawn = this.GetOwningPlayerPawn();
            pawn.ArrowComponent_EditorOnly.SetHiddenInGame(true, true);
            pawn.ArrowComponent_EditorOnly.SetIsScreenSizeScaled(false);
            pawn.AttackHitbox.SetHiddenInGame(true, true);
            pawn.AttackHitbox.SetIsScreenSizeScaled(false);
        }
    }

    Get_IsDebug_CheckedState(): UE.ECheckBoxState {
        return this.GetWorld().OwningGameInstance.bDebug ? UE.ECheckBoxState.Checked : UE.ECheckBoxState.Unchecked;
    }

    BndEvt__W_Debug_IsDebug_K2Node_ComponentBoundEvent_0_OnCheckBoxComponentStateChanged__DelegateSignature(bIsChecked: boolean) : void {
        this.GetWorld().OwningGameInstance.bDebug = bIsChecked;
        this.Debug();
    }

}

blueprint.mixin(jsClass, TS_Debug);