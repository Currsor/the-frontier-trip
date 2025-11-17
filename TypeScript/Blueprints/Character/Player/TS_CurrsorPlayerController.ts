import * as UE from 'ue';
import { $ref, blueprint } from 'puerts';
import { gameSystemManager } from '../../../GameSystemManager';
import { AttackSystem } from '../../../Systems/AttackSystem';
import { EventSystem } from '../../../Systems/EventSystem';

const uclass = UE.Class.Load("/Game/Blueprints/Character/Player/BP_CurrsorPlayerController.BP_CurrsorPlayerController_C");

const jsClass = blueprint.tojs<typeof UE.Game.Blueprints.Character.Player.BP_CurrsorPlayerController.BP_CurrsorPlayerController_C>(uclass);

class TS_CurrsorPlayerController extends jsClass {
    static CurrsorPlayer: UE.CurrsorCharacter;
    static CurrsorPlayerState: UE.CurrsorPlayerState;
    
    private attackSystem!: AttackSystem;
    private isSystemsInitialized: boolean = false;

    ReceiveBeginPlay(): void {
        TS_CurrsorPlayerController.CurrsorPlayer = UE.GameplayStatics.GetPlayerPawn(this, 0) as UE.CurrsorCharacter;
        TS_CurrsorPlayerController.CurrsorPlayerState = TS_CurrsorPlayerController.CurrsorPlayer.PlayerState as UE.CurrsorPlayerState;

        // 初始化游戏系统
        this.initializeGameSystems();

        // 在游戏开始时添加调试小部件到视口
        this.AddDebugWidgetToViewport();
    }

    private initializeGameSystems(): void {
        try {
            // 确保游戏系统管理器已初始化
            if (!gameSystemManager.isSystemInitialized("AttackSystem")) {
                gameSystemManager.initialize();
            }

            // 获取系统实例
            this.attackSystem = gameSystemManager.getSystem("AttackSystem");
            
            this.isSystemsInitialized = true;
            console.log("TS_CurrsorPlayerController: Game systems initialized successfully");

            // 设置输入事件监听
            this.setupInputEventListeners();

        } catch (error) {
            console.error("TS_CurrsorPlayerController: Failed to initialize game systems:", error);
        }
    }

    private setupInputEventListeners(): void {
        // 监听攻击输入事件
        EventSystem.subscribe("onAttackInputProcessed", (data: any) => {
            console.log("TS: Attack input processed", data);
        });

        // 监听状态变化事件
        EventSystem.subscribe("onStateChanged", (data: any) => {
            console.log(`TS: State changed from ${data.oldState} to ${data.newState}`);
        });
    }

    private AddDebugWidgetToViewport(): void {
        blueprint.load(UE.Game.UI.Blueprints.Debug.W_Debug.W_Debug_C);

        const PlayerController = UE.GameplayStatics.GetPlayerController(this, 0);
        const debugWidget = UE.WidgetBlueprintLibrary.Create(this, UE.Game.UI.Blueprints.Debug.W_Debug.W_Debug_C.StaticClass(), PlayerController);

        debugWidget.AddToViewport();

        blueprint.unload(UE.Game.UI.Blueprints.Debug.W_Debug.W_Debug_C);
    }

    // 攻击输入处理
    public onAttackInput(): void {
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
    public onMoveInput(inputVector: UE.Vector): void {
        if (!this.isSystemsInitialized) return;

        console.log("TS: Move input received", inputVector);
        
        // 广播移动输入事件
        EventSystem.emit("onMoveInput", {
            inputVector: inputVector,
            player: TS_CurrsorPlayerController.CurrsorPlayer,
            timestamp: Date.now()
        });

        // 根据移动输入更新状态
        this.updateMovementState(inputVector);
    }

    // 更新移动状态
    private updateMovementState(inputVector: UE.Vector): void {
        if (!inputVector) return;

        const inputMagnitude = Math.sqrt(inputVector.X * inputVector.X + inputVector.Y * inputVector.Y);
        const playerActor = TS_CurrsorPlayerController.CurrsorPlayer;
        
        if (inputMagnitude > 0.1) {
            // 根据输入强度决定是走路还是跑步
            const targetState = inputMagnitude > 0.8 ? "Run" : "Walk";
            
            // 通过事件系统请求C++层进行状态转换
            EventSystem.emit("onRequestStateChange", {
                actor: playerActor,
                requestedState: targetState,
                source: "PlayerController"
            });
        } else {
            // 没有输入时回到Idle状态
            EventSystem.emit("onRequestStateChange", {
                actor: playerActor,
                requestedState: "Idle",
                source: "PlayerController"
            });
        }
    }
}

blueprint.mixin(jsClass, TS_CurrsorPlayerController);