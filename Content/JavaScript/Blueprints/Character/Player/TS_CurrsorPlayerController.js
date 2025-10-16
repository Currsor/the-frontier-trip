"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const UE = require("ue");
const puerts_1 = require("puerts");
const GameSystemManager_1 = require("../../../GameSystemManager");
const EventSystem_1 = require("../../../Systems/EventSystem");
const uclass = UE.Class.Load("/Game/Blueprints/Character/Player/BP_CurrsorPlayerController.BP_CurrsorPlayerController_C");
const jsClass = puerts_1.blueprint.tojs(uclass);
class TS_CurrsorPlayerController extends jsClass {
    static CurrsorPlayer;
    static CurrsorPlayerState;
    attackSystem;
    isSystemsInitialized = false;
    ReceiveBeginPlay() {
        TS_CurrsorPlayerController.CurrsorPlayer = UE.GameplayStatics.GetPlayerPawn(this, 0);
        TS_CurrsorPlayerController.CurrsorPlayerState = TS_CurrsorPlayerController.CurrsorPlayer.PlayerState;
        // 初始化游戏系统
        this.initializeGameSystems();
        // 在游戏开始时添加调试小部件到视口
        this.AddDebugWidgetToViewport();
    }
    initializeGameSystems() {
        try {
            // 确保游戏系统管理器已初始化
            if (!GameSystemManager_1.gameSystemManager.isSystemInitialized("AttackSystem")) {
                GameSystemManager_1.gameSystemManager.initialize();
            }
            // 获取系统实例
            this.attackSystem = GameSystemManager_1.gameSystemManager.getSystem("AttackSystem");
            this.isSystemsInitialized = true;
            console.log("TS_CurrsorPlayerController: Game systems initialized successfully");
            // 设置输入事件监听
            this.setupInputEventListeners();
        }
        catch (error) {
            console.error("TS_CurrsorPlayerController: Failed to initialize game systems:", error);
        }
    }
    setupInputEventListeners() {
        // 监听攻击输入事件
        EventSystem_1.EventSystem.subscribe("onAttackInputProcessed", (data) => {
            console.log("TS: Attack input processed", data);
        });
        // 监听状态变化事件
        EventSystem_1.EventSystem.subscribe("onStateChanged", (data) => {
            console.log(`TS: State changed from ${data.oldState} to ${data.newState}`);
        });
    }
    AddDebugWidgetToViewport() {
        puerts_1.blueprint.load(UE.Game.UMG.Debug.W_Debug.W_Debug_C);
        const PlayerController = UE.GameplayStatics.GetPlayerController(this, 0);
        const debugWidget = UE.WidgetBlueprintLibrary.Create(this, UE.Game.UMG.Debug.W_Debug.W_Debug_C.StaticClass(), PlayerController);
        debugWidget.AddToViewport();
        puerts_1.blueprint.unload(UE.Game.UMG.Debug.W_Debug.W_Debug_C);
    }
    // 攻击输入处理
    onAttackInput() {
        if (!this.isSystemsInitialized) {
            console.log("TS: Systems not initialized, skipping attack input");
            return;
        }
        console.log("TS: Attack input received - forwarding directly to C++");
        const playerActor = TS_CurrsorPlayerController.CurrsorPlayer;
        if (playerActor) {
            // 直接调用 C++ 攻击系统，不通过事件系统
            this.attackSystem.handleAttackInput(playerActor);
        }
    }
    // 移动输入处理
    onMoveInput(inputVector) {
        if (!this.isSystemsInitialized)
            return;
        console.log("TS: Move input received", inputVector);
        // 广播移动输入事件
        EventSystem_1.EventSystem.emit("onMoveInput", {
            inputVector: inputVector,
            player: TS_CurrsorPlayerController.CurrsorPlayer,
            timestamp: Date.now()
        });
        // 根据移动输入更新状态
        this.updateMovementState(inputVector);
    }
    // 跳跃输入处理
    onJumpInput() {
        if (!this.isSystemsInitialized)
            return;
        console.log("TS: Jump input received");
        const playerActor = TS_CurrsorPlayerController.CurrsorPlayer;
        if (playerActor && this.canJump()) {
            EventSystem_1.EventSystem.emit("onJumpInput", {
                player: playerActor,
                timestamp: Date.now()
            });
            // 通过事件系统请求C++层进行状态转换
            EventSystem_1.EventSystem.emit("onRequestStateChange", {
                actor: playerActor,
                requestedState: "Jump",
                source: "PlayerController"
            });
        }
    }
    // 冲刺输入处理
    onDashInput() {
        if (!this.isSystemsInitialized)
            return;
        console.log("TS: Dash input received");
        const playerActor = TS_CurrsorPlayerController.CurrsorPlayer;
        if (playerActor && this.canDash()) {
            EventSystem_1.EventSystem.emit("onDashInput", {
                player: playerActor,
                timestamp: Date.now()
            });
            // 通过事件系统请求C++层进行状态转换
            EventSystem_1.EventSystem.emit("onRequestStateChange", {
                actor: playerActor,
                requestedState: "Dash",
                source: "PlayerController"
            });
        }
    }
    // 更新移动状态
    updateMovementState(inputVector) {
        if (!inputVector)
            return;
        const inputMagnitude = Math.sqrt(inputVector.X * inputVector.X + inputVector.Y * inputVector.Y);
        const playerActor = TS_CurrsorPlayerController.CurrsorPlayer;
        if (inputMagnitude > 0.1) {
            // 根据输入强度决定是走路还是跑步
            const targetState = inputMagnitude > 0.8 ? "Run" : "Walk";
            // 通过事件系统请求C++层进行状态转换
            EventSystem_1.EventSystem.emit("onRequestStateChange", {
                actor: playerActor,
                requestedState: targetState,
                source: "PlayerController"
            });
        }
        else {
            // 没有输入时回到Idle状态
            EventSystem_1.EventSystem.emit("onRequestStateChange", {
                actor: playerActor,
                requestedState: "Idle",
                source: "PlayerController"
            });
        }
    }
    // 检查是否可以跳跃
    canJump() {
        // 跳跃检查由C++层的StateManagerComponent处理
        // 这里暂时返回true，具体逻辑由C++层决定
        return true;
    }
    // 检查是否可以冲刺
    canDash() {
        // 冲刺检查由C++层的StateManagerComponent处理
        // 这里暂时返回true，具体逻辑由C++层决定
        return true;
    }
    // 获取当前状态信息（供调试使用）
    getStateInfo() {
        // 状态信息由C++层的StateManagerComponent提供
        return { message: "State info now handled by C++ StateManagerComponent" };
    }
    // 获取攻击统计信息（供调试使用）
    getAttackInfo() {
        if (!this.attackSystem)
            return null;
        return this.attackSystem.getAttackStats();
    }
    // 获取系统状态（供调试使用）
    getSystemStatus() {
        return GameSystemManager_1.gameSystemManager.getSystemStatus();
    }
    // 重置控制器状态
    resetController() {
        if (this.attackSystem) {
            this.attackSystem.reset();
        }
        // 通过事件系统请求C++层重置状态
        EventSystem_1.EventSystem.emit("onRequestSystemReset", {
            system: "StateManager",
            source: "PlayerController"
        });
        console.log("TS_CurrsorPlayerController reset");
    }
}
puerts_1.blueprint.mixin(jsClass, TS_CurrsorPlayerController);
//# sourceMappingURL=TS_CurrsorPlayerController.js.map