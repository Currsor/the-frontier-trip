import * as UE from 'ue';
import { $ref, blueprint } from 'puerts';

const uclass = UE.Class.Load("/Game/Blueprints/Character/Player/BP_CurrsorCharacter.BP_CurrsorCharacter_C");
const jsClass = blueprint.tojs<typeof UE.Game.Blueprints.Character.Player.BP_CurrsorCharacter.BP_CurrsorCharacter_C>(uclass);

class TS_CurrsorCharacter extends jsClass {

    private State : UE.EPlayerState = UE.EPlayerState.Idle;


    ReceiveTick(): void {
        this.Debug();
    }
    
    // Debug
    public Debug(): void {
        if (this.GetWorld().OwningGameInstance.bIsDebug) {
            this.ArrowComponent_EditorOnly.SetHiddenInGame(false, true);
            this.ArrowComponent_EditorOnly.SetIsScreenSizeScaled(true);

            this.State = this.PlayerState.GetCurrentState();
            UE.KismetSystemLibrary.PrintString(this, "PlayerState: " + UE.EPlayerState[this.State], true, false, UE.LinearColor.White, 0.0);
        }
    }

}

blueprint.mixin(jsClass, TS_CurrsorCharacter);