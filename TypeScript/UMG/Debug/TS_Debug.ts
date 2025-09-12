import * as UE from 'ue';
import { $ref, blueprint } from 'puerts';
import { gameSystemManager } from '../../GameSystemManager';
import { AttackSystem } from '../../Systems/AttackSystem';
import { StateManager } from '../../Managers/StateManager';
import { EventSystem } from '../../Systems/EventSystem';
import { GameLogicManager } from '../../Managers/GameLogicManager';
import { UIManager } from '../../Managers/UIManager';
import { LootSystem } from '../../Systems/LootSystem';

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
    static GameState: UE.CurrsorGameState;
    
    // 新架构系统引用
    private attackSystem!: AttackSystem;
    private stateManager!: StateManager;
    private gameLogicManager!: GameLogicManager;
    private uiManager!: UIManager;
    private lootSystem!: LootSystem;
    private isSystemsInitialized: boolean = false;

    Construct() {
        TS_Debug.pawn = this.GetOwningPlayerPawn() as UE.CurrsorCharacter;
        TS_Debug.PlayerState = TS_Debug.pawn.PlayerState as UE.CurrsorPlayerState;
        TS_Debug.GameInstance = this.GetWorld().OwningGameInstance as UE.CurrsorGameInstance;
        TS_Debug.GameState = this.GetWorld().GameState as UE.CurrsorGameState;
        
        // 初始化新架构系统
        this.initializeNewSystems();
        
        this.Debug();
    }

    private initializeNewSystems(): void {
        try {
            // 确保游戏系统管理器已初始化
            if (!gameSystemManager.isSystemInitialized("AttackSystem")) {
                gameSystemManager.initialize();
            }

            // 获取系统实例
            this.attackSystem = gameSystemManager.getSystem("AttackSystem");
            this.stateManager = gameSystemManager.getSystem("StateManager");
            this.gameLogicManager = gameSystemManager.getSystem("GameLogicManager");
            this.uiManager = gameSystemManager.getSystem("UIManager");
            this.lootSystem = gameSystemManager.getSystem("LootSystem");
            
            this.isSystemsInitialized = true;
            console.log("TS_Debug: New architecture systems initialized successfully");

        } catch (error) {
            console.error("TS_Debug: Failed to initialize new architecture systems:", error);
        }
    }

    Tick(MyGeometry: UE.Geometry, InDeltaTime: number): void {
        if (!TS_Debug.GameInstance.bAttackDebug) return;

        if(TS_Debug.pawn.AttackHitbox.GetCollisionProfileName() == "OverlapAll") {
            TS_Debug.pawn.AttackHitbox.SetHiddenInGame(false, true);
        }
        else {
            TS_Debug.pawn.AttackHitbox.SetHiddenInGame(true, true);
        }

    }
    
    // Debug
    public Debug(): void {
        if (TS_Debug.GameInstance.bDebug) {
            TS_Debug.pawn.ArrowComponent_EditorOnly.SetHiddenInGame(false, true);
            TS_Debug.pawn.ArrowComponent_EditorOnly.SetIsScreenSizeScaled(true);
            
            this.Get_State_Text();
            this.Get_ID_Text();
            // UE.KismetSystemLibrary.PrintString(this, "PlayerState: " + UE.EPlayerState[this.TS_State], true, false, UE.LinearColor.White, 0.0);

            this.Overlay_DebugAttack.SetVisibility(UE.ESlateVisibility.Visible);
        } 
        else {
            TS_Debug.pawn.ArrowComponent_EditorOnly.SetHiddenInGame(true, true);
            TS_Debug.pawn.ArrowComponent_EditorOnly.SetIsScreenSizeScaled(false);
            TS_Debug.pawn.AttackHitbox.SetHiddenInGame(true, true);

            this.Overlay_DebugAttack.SetVisibility(UE.ESlateVisibility.Hidden);
        }
    }

    Get_IsDebug_CheckedState(): UE.ECheckBoxState {
        return TS_Debug.GameInstance.bDebug ? UE.ECheckBoxState.Checked : UE.ECheckBoxState.Unchecked;
    }

    Get_IsDebug_Attack_CheckedState(): UE.ECheckBoxState {
        return TS_Debug.GameInstance.bAttackDebug ? UE.ECheckBoxState.Checked : UE.ECheckBoxState.Unchecked;
    }

    Get_State_Text(): string {
        let stateText = "";
        
        // 新架构状态信息
        if (this.isSystemsInitialized && this.stateManager) {
            const currentState = this.stateManager.getCurrentState();
            const stateDuration = this.stateManager.getStateDuration();
            stateText += `TS: ${currentState} (${stateDuration}ms)`;
        }
        
        // 原有系统状态信息
        if (TS_Debug.PlayerState) {
            const stateNum = TS_Debug.PlayerState.GetCurrentState();
            const legacyState = TS_Debug.EPlayerStateMap[stateNum] ?? stateNum.toString();
            stateText += stateText ? ` | C++: ${legacyState}` : `C++: ${legacyState}`;
        }
        
        return stateText || "Unknown";
    }

    Get_ID_Text() : string {
        if (TS_Debug.GameState.GetCurrentAreaID() == 0) return "None";
        const actor = TS_Debug.GameState.GetNameFromID(TS_Debug.GameState.GetCurrentAreaID());
        return actor ? actor : "Invalid";
    }
    

    BndEvt__W_Debug_IsDebug_K2Node_ComponentBoundEvent_0_OnCheckBoxComponentStateChanged__DelegateSignature(bIsChecked: boolean) : void {
        TS_Debug.GameInstance.bDebug = bIsChecked;
        this.Debug();
    }

    BndEvt__W_Debug_IsDebug_Attack_K2Node_ComponentBoundEvent_1_OnCheckBoxComponentStateChanged__DelegateSignature(bIsChecked: boolean) : void {
        TS_Debug.GameInstance.bAttackDebug = bIsChecked;
        this.Debug();
    }

    BndEvt__W_Debug_Button_EnterBattle_K2Node_ComponentBoundEvent_2_OnButtonClickedEvent__DelegateSignature() : void {
        
    }

    // 新架构系统调试方法
    
    // 获取攻击系统信息
    Get_Attack_System_Info(): string {
        if (!this.isSystemsInitialized || !this.attackSystem) {
            return "Attack System: Not initialized";
        }
        
        const attackStats = this.attackSystem.getAttackStats();
        return `Attack: Combo=${attackStats.attackComboCount}, LastAttack=${attackStats.timeSinceLastAttack}ms ago`;
    }

    // 获取事件系统信息
    Get_Event_System_Info(): string {
        if (!this.isSystemsInitialized) {
            return "Event System: Not initialized";
        }
        
        const registeredEvents = EventSystem.getRegisteredEvents();
        const recentEvents = EventSystem.getEventHistory(undefined, 3);
        
        let info = `Events: ${registeredEvents.length} types`;
        if (recentEvents.length > 0) {
            const recentEventNames = recentEvents.map(e => e.event).join(", ");
            info += ` | Recent: ${recentEventNames}`;
        }
        
        return info;
    }

    // 获取掉落系统信息
    Get_Loot_System_Info(): string {
        if (!this.isSystemsInitialized || !this.lootSystem) {
            return "Loot System: Not initialized";
        }
        
        const dropStats = this.lootSystem.getDropStats();
        return `Loot: ${dropStats.totalDrops} drops | Recent: ${dropStats.recentDrops.length}`;
    }

    // 获取UI系统信息
    Get_UI_System_Info(): string {
        if (!this.isSystemsInitialized || !this.uiManager) {
            return "UI System: Not initialized";
        }
        
        const uiStats = this.uiManager.getUIStats();
        return `UI: ${uiStats.activeDamageNumbers} damage numbers, ${uiStats.activeNotifications} notifications`;
    }

    // 获取系统架构概览
    Get_System_Architecture_Info(): string {
        if (!this.isSystemsInitialized) {
            return "New Architecture: Not initialized";
        }
        
        const systemStatus = gameSystemManager.getSystemStatus();
        return `Architecture: ${systemStatus.systemCount} systems active, ${systemStatus.eventStats.registeredEvents} event types`;
    }

    // 测试新架构功能的按钮事件

    // 测试攻击系统
    Test_Attack_System(): void {
        console.log("Testing Attack System");
        
        if (this.isSystemsInitialized && TS_Debug.pawn) {
            EventSystem.emit("onAttackInput", {
                attacker: TS_Debug.pawn,
                inputType: "debug_test",
                timestamp: Date.now()
            });
        }
    }

    // 测试状态转换
    Test_State_Transition(): void {
        console.log("Testing State Transition");
        
        if (this.isSystemsInitialized && this.stateManager) {
            const currentState = this.stateManager.getCurrentState();
            const testState = currentState === StateManager.STATES.IDLE ? 
                StateManager.STATES.ATTACK : StateManager.STATES.IDLE;
            
            this.stateManager.changeState(testState);
        }
    }

    // 测试掉落系统
    Test_Loot_System(): void {
        console.log("Testing Loot System");
        
        if (this.isSystemsInitialized && this.lootSystem && TS_Debug.pawn) {
            this.lootSystem.generateLoot(TS_Debug.pawn, "DestructibleItem", 1.0);
        }
    }

    // 测试伤害数字显示
    Test_Damage_Numbers(): void {
        console.log("Testing Damage Numbers");
        
        if (this.isSystemsInitialized && this.uiManager && TS_Debug.pawn) {
            const playerLocation = TS_Debug.pawn.K2_GetActorLocation();
            const damage = Math.floor(Math.random() * 50) + 10;
            const isCritical = Math.random() > 0.7;
            
            this.uiManager.showDamageNumber(damage, playerLocation, isCritical);
        }
    }

    // 重置所有系统
    Reset_All_Systems(): void {
        console.log("Resetting All Systems");
        
        if (this.isSystemsInitialized) {
            gameSystemManager.resetAllSystems();
        }
    }

    // 打印系统详细信息到控制台
    Print_System_Details(): void {
        console.log("=== System Details ===");
        
        if (this.isSystemsInitialized) {
            gameSystemManager.debugPrintStatus();
            
            console.log("Attack System:", this.Get_Attack_System_Info());
            console.log("Event System:", this.Get_Event_System_Info());
            console.log("Loot System:", this.Get_Loot_System_Info());
            console.log("UI System:", this.Get_UI_System_Info());
        } else {
            console.log("New architecture systems not initialized");
        }
        
        console.log("======================");
    }

    // 获取完整的调试信息字符串
    Get_Complete_Debug_Info(): string {
        let debugInfo = "=== Complete Debug Info ===\n";
        
        // 基础信息
        debugInfo += `State: ${this.Get_State_Text()}\n`;
        debugInfo += `Area ID: ${this.Get_ID_Text()}\n`;
        
        // 新架构系统信息
        if (this.isSystemsInitialized) {
            debugInfo += `Architecture: ${this.Get_System_Architecture_Info()}\n`;
            debugInfo += `Attack: ${this.Get_Attack_System_Info()}\n`;
            debugInfo += `Events: ${this.Get_Event_System_Info()}\n`;
            debugInfo += `Loot: ${this.Get_Loot_System_Info()}\n`;
            debugInfo += `UI: ${this.Get_UI_System_Info()}\n`;
        } else {
            debugInfo += "New Architecture: Not initialized\n";
        }
        
        debugInfo += "===========================";
        
        return debugInfo;
    }

}

blueprint.mixin(jsClass, TS_Debug);