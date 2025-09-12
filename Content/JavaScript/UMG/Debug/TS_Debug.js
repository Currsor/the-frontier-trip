"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const UE = require("ue");
const puerts_1 = require("puerts");
const GameSystemManager_1 = require("../../GameSystemManager");
const StateManager_1 = require("../../Managers/StateManager");
const EventSystem_1 = require("../../Systems/EventSystem");
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
    static GameState;
    // 新架构系统引用
    attackSystem;
    stateManager;
    gameLogicManager;
    uiManager;
    lootSystem;
    isSystemsInitialized = false;
    Construct() {
        TS_Debug.pawn = this.GetOwningPlayerPawn();
        TS_Debug.PlayerState = TS_Debug.pawn.PlayerState;
        TS_Debug.GameInstance = this.GetWorld().OwningGameInstance;
        TS_Debug.GameState = this.GetWorld().GameState;
        // 初始化新架构系统
        this.initializeNewSystems();
        this.Debug();
    }
    initializeNewSystems() {
        try {
            // 确保游戏系统管理器已初始化
            if (!GameSystemManager_1.gameSystemManager.isSystemInitialized("AttackSystem")) {
                GameSystemManager_1.gameSystemManager.initialize();
            }
            // 获取系统实例
            this.attackSystem = GameSystemManager_1.gameSystemManager.getSystem("AttackSystem");
            this.stateManager = GameSystemManager_1.gameSystemManager.getSystem("StateManager");
            this.gameLogicManager = GameSystemManager_1.gameSystemManager.getSystem("GameLogicManager");
            this.uiManager = GameSystemManager_1.gameSystemManager.getSystem("UIManager");
            this.lootSystem = GameSystemManager_1.gameSystemManager.getSystem("LootSystem");
            this.isSystemsInitialized = true;
            console.log("TS_Debug: New architecture systems initialized successfully");
        }
        catch (error) {
            console.error("TS_Debug: Failed to initialize new architecture systems:", error);
        }
    }
    Tick(MyGeometry, InDeltaTime) {
        if (!TS_Debug.GameInstance.bAttackDebug)
            return;
        if (TS_Debug.pawn.AttackHitbox.GetCollisionProfileName() == "OverlapAll") {
            TS_Debug.pawn.AttackHitbox.SetHiddenInGame(false, true);
        }
        else {
            TS_Debug.pawn.AttackHitbox.SetHiddenInGame(true, true);
        }
    }
    // Debug
    Debug() {
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
    Get_IsDebug_CheckedState() {
        return TS_Debug.GameInstance.bDebug ? UE.ECheckBoxState.Checked : UE.ECheckBoxState.Unchecked;
    }
    Get_IsDebug_Attack_CheckedState() {
        return TS_Debug.GameInstance.bAttackDebug ? UE.ECheckBoxState.Checked : UE.ECheckBoxState.Unchecked;
    }
    Get_State_Text() {
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
    Get_ID_Text() {
        if (TS_Debug.GameState.GetCurrentAreaID() == 0)
            return "None";
        const actor = TS_Debug.GameState.GetNameFromID(TS_Debug.GameState.GetCurrentAreaID());
        return actor ? actor : "Invalid";
    }
    BndEvt__W_Debug_IsDebug_K2Node_ComponentBoundEvent_0_OnCheckBoxComponentStateChanged__DelegateSignature(bIsChecked) {
        TS_Debug.GameInstance.bDebug = bIsChecked;
        this.Debug();
    }
    BndEvt__W_Debug_IsDebug_Attack_K2Node_ComponentBoundEvent_1_OnCheckBoxComponentStateChanged__DelegateSignature(bIsChecked) {
        TS_Debug.GameInstance.bAttackDebug = bIsChecked;
        this.Debug();
    }
    BndEvt__W_Debug_Button_EnterBattle_K2Node_ComponentBoundEvent_2_OnButtonClickedEvent__DelegateSignature() {
    }
    // 新架构系统调试方法
    // 获取攻击系统信息
    Get_Attack_System_Info() {
        if (!this.isSystemsInitialized || !this.attackSystem) {
            return "Attack System: Not initialized";
        }
        const attackStats = this.attackSystem.getAttackStats();
        return `Attack: Combo=${attackStats.attackComboCount}, LastAttack=${attackStats.timeSinceLastAttack}ms ago`;
    }
    // 获取事件系统信息
    Get_Event_System_Info() {
        if (!this.isSystemsInitialized) {
            return "Event System: Not initialized";
        }
        const registeredEvents = EventSystem_1.EventSystem.getRegisteredEvents();
        const recentEvents = EventSystem_1.EventSystem.getEventHistory(undefined, 3);
        let info = `Events: ${registeredEvents.length} types`;
        if (recentEvents.length > 0) {
            const recentEventNames = recentEvents.map(e => e.event).join(", ");
            info += ` | Recent: ${recentEventNames}`;
        }
        return info;
    }
    // 获取掉落系统信息
    Get_Loot_System_Info() {
        if (!this.isSystemsInitialized || !this.lootSystem) {
            return "Loot System: Not initialized";
        }
        const dropStats = this.lootSystem.getDropStats();
        return `Loot: ${dropStats.totalDrops} drops | Recent: ${dropStats.recentDrops.length}`;
    }
    // 获取UI系统信息
    Get_UI_System_Info() {
        if (!this.isSystemsInitialized || !this.uiManager) {
            return "UI System: Not initialized";
        }
        const uiStats = this.uiManager.getUIStats();
        return `UI: ${uiStats.activeDamageNumbers} damage numbers, ${uiStats.activeNotifications} notifications`;
    }
    // 获取系统架构概览
    Get_System_Architecture_Info() {
        if (!this.isSystemsInitialized) {
            return "New Architecture: Not initialized";
        }
        const systemStatus = GameSystemManager_1.gameSystemManager.getSystemStatus();
        return `Architecture: ${systemStatus.systemCount} systems active, ${systemStatus.eventStats.registeredEvents} event types`;
    }
    // 测试新架构功能的按钮事件
    // 测试攻击系统
    Test_Attack_System() {
        console.log("Testing Attack System");
        if (this.isSystemsInitialized && TS_Debug.pawn) {
            EventSystem_1.EventSystem.emit("onAttackInput", {
                attacker: TS_Debug.pawn,
                inputType: "debug_test",
                timestamp: Date.now()
            });
        }
    }
    // 测试状态转换
    Test_State_Transition() {
        console.log("Testing State Transition");
        if (this.isSystemsInitialized && this.stateManager) {
            const currentState = this.stateManager.getCurrentState();
            const testState = currentState === StateManager_1.StateManager.STATES.IDLE ?
                StateManager_1.StateManager.STATES.ATTACK : StateManager_1.StateManager.STATES.IDLE;
            this.stateManager.changeState(testState);
        }
    }
    // 测试掉落系统
    Test_Loot_System() {
        console.log("Testing Loot System");
        if (this.isSystemsInitialized && this.lootSystem && TS_Debug.pawn) {
            this.lootSystem.generateLoot(TS_Debug.pawn, "DestructibleItem", 1.0);
        }
    }
    // 测试伤害数字显示
    Test_Damage_Numbers() {
        console.log("Testing Damage Numbers");
        if (this.isSystemsInitialized && this.uiManager && TS_Debug.pawn) {
            const playerLocation = TS_Debug.pawn.K2_GetActorLocation();
            const damage = Math.floor(Math.random() * 50) + 10;
            const isCritical = Math.random() > 0.7;
            this.uiManager.showDamageNumber(damage, playerLocation, isCritical);
        }
    }
    // 重置所有系统
    Reset_All_Systems() {
        console.log("Resetting All Systems");
        if (this.isSystemsInitialized) {
            GameSystemManager_1.gameSystemManager.resetAllSystems();
        }
    }
    // 打印系统详细信息到控制台
    Print_System_Details() {
        console.log("=== System Details ===");
        if (this.isSystemsInitialized) {
            GameSystemManager_1.gameSystemManager.debugPrintStatus();
            console.log("Attack System:", this.Get_Attack_System_Info());
            console.log("Event System:", this.Get_Event_System_Info());
            console.log("Loot System:", this.Get_Loot_System_Info());
            console.log("UI System:", this.Get_UI_System_Info());
        }
        else {
            console.log("New architecture systems not initialized");
        }
        console.log("======================");
    }
    // 获取完整的调试信息字符串
    Get_Complete_Debug_Info() {
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
        }
        else {
            debugInfo += "New Architecture: Not initialized\n";
        }
        debugInfo += "===========================";
        return debugInfo;
    }
}
puerts_1.blueprint.mixin(jsClass, TS_Debug);
//# sourceMappingURL=TS_Debug.js.map