"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const UE = require("ue");
const puerts_1 = require("puerts");
const GameSystemManager_1 = require("../../../GameSystemManager");
const StateManager_1 = require("../../../Managers/StateManager");
const EventSystem_1 = require("../../../Systems/EventSystem");
const uclass = UE.Class.Load("/Game/Blueprints/Character/Player/BP_CurrsorPlayerController.BP_CurrsorPlayerController_C");
const jsClass = puerts_1.blueprint.tojs(uclass);
class TS_CurrsorPlayerController extends jsClass {
    static CurrsorPlayer;
    static CurrsorPlayerState;
    attackSystem;
    stateManager;
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
            this.stateManager = GameSystemManager_1.gameSystemManager.getSystem("StateManager");
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
        console.log("TS: Attack input received");
        const playerActor = TS_CurrsorPlayerController.CurrsorPlayer;
        if (playerActor) {
            // 广播攻击输入事件，让攻击系统处理
            EventSystem_1.EventSystem.emit("onAttackInput", {
                attacker: playerActor,
                inputType: "primary",
                timestamp: Date.now()
            });
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
            // 尝试转换到跳跃状态
            this.stateManager.changeState(StateManager_1.StateManager.STATES.JUMP);
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
            // 尝试转换到冲刺状态
            this.stateManager.changeState(StateManager_1.StateManager.STATES.DASH);
        }
    }
    // 更新移动状态
    updateMovementState(inputVector) {
        if (!inputVector || !this.stateManager)
            return;
        const inputMagnitude = Math.sqrt(inputVector.X * inputVector.X + inputVector.Y * inputVector.Y);
        if (inputMagnitude > 0.1) {
            // 根据输入强度决定是走路还是跑步
            const targetState = inputMagnitude > 0.8 ?
                StateManager_1.StateManager.STATES.RUN : StateManager_1.StateManager.STATES.WALK;
            // 只有在不是攻击状态时才改变移动状态
            if (!this.stateManager.isAttacking()) {
                this.stateManager.changeState(targetState);
            }
        }
        else {
            // 没有输入时回到Idle状态
            if (this.stateManager.isMoving() && !this.stateManager.isAttacking()) {
                this.stateManager.changeState(StateManager_1.StateManager.STATES.IDLE);
            }
        }
    }
    // 检查是否可以跳跃
    canJump() {
        if (!this.stateManager)
            return false;
        // 死亡或受伤时不能跳跃
        if (this.stateManager.isDead() || this.stateManager.isHurt()) {
            return false;
        }
        // 已经在跳跃或下落时不能再次跳跃
        const currentState = this.stateManager.getCurrentState();
        if (currentState === StateManager_1.StateManager.STATES.JUMP || currentState === StateManager_1.StateManager.STATES.FALL) {
            return false;
        }
        return true;
    }
    // 检查是否可以冲刺
    canDash() {
        if (!this.stateManager)
            return false;
        // 死亡或受伤时不能冲刺
        if (this.stateManager.isDead() || this.stateManager.isHurt()) {
            return false;
        }
        // 已经在冲刺时不能再次冲刺
        if (this.stateManager.isDashing()) {
            return false;
        }
        return true;
    }
    // 获取当前状态信息（供调试使用）
    getStateInfo() {
        if (!this.stateManager)
            return null;
        return this.stateManager.getStateStats();
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
        if (this.stateManager) {
            this.stateManager.reset();
        }
        if (this.attackSystem) {
            this.attackSystem.reset();
        }
        console.log("TS_CurrsorPlayerController reset");
    }
}
puerts_1.blueprint.mixin(jsClass, TS_CurrsorPlayerController);
//# sourceMappingURL=TS_CurrsorPlayerController.js.map