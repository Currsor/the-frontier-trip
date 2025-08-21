import * as UE from 'ue';
import { $ref, blueprint } from 'puerts';

const uclass = UE.Class.Load("/Game/Blueprints/Character/Player/BP_CurrsorPlayerController.BP_CurrsorPlayerController_C");

const jsClass = blueprint.tojs<typeof UE.Game.Blueprints.Character.Player.BP_CurrsorPlayerController.BP_CurrsorPlayerController_C>(uclass);

class TS_CurrsorPlayerController extends jsClass {
    ReceiveBeginPlay(): void {

        // 在游戏开始时添加调试小部件到视口
        this.AddDebugWidgetToViewport();
        
    }

    private AddDebugWidgetToViewport(): void {
        blueprint.load(UE.Game.UMG.Debug.W_Debug.W_Debug_C);

        const PlayerController = UE.GameplayStatics.GetPlayerController(this, 0);
        const debugWidget = UE.WidgetBlueprintLibrary.Create(this, UE.Game.UMG.Debug.W_Debug.W_Debug_C.StaticClass(), PlayerController);

        debugWidget.AddToViewport();

        blueprint.unload(UE.Game.UMG.Debug.W_Debug.W_Debug_C);
    }
}

blueprint.mixin(jsClass, TS_CurrsorPlayerController);