"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const UE = require("ue");
const puerts_1 = require("puerts");
const uclass = UE.Class.Load("/Game/Blueprints/Character/Player/BP_CurrsorPlayerController.BP_CurrsorPlayerController_C");
const jsClass = puerts_1.blueprint.tojs(uclass);
class TS_CurrsorPlayerController extends jsClass {
    ReceiveBeginPlay() {
        // 在游戏开始时添加调试小部件到视口
        this.AddDebugWidgetToViewport();
    }
    AddDebugWidgetToViewport() {
        puerts_1.blueprint.load(UE.Game.UMG.Debug.W_Debug.W_Debug_C);
        const PlayerController = UE.GameplayStatics.GetPlayerController(this, 0);
        const debugWidget = UE.WidgetBlueprintLibrary.Create(this, UE.Game.UMG.Debug.W_Debug.W_Debug_C.StaticClass(), PlayerController);
        debugWidget.AddToViewport();
        puerts_1.blueprint.unload(UE.Game.UMG.Debug.W_Debug.W_Debug_C);
    }
}
puerts_1.blueprint.mixin(jsClass, TS_CurrsorPlayerController);
//# sourceMappingURL=TS_CurrsorPlayerController.js.map