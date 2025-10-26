"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const UE = require("ue");
const puerts_1 = require("puerts");
const GameSystemManager_1 = require("../../GameSystemManager");
const EventSystem_1 = require("../../Systems/EventSystem");
const uclass = UE.Class.Load("/Game/UI/Blueprints/Debug/W_Debug.W_Debug_C");
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
            this.gameLogicManager = GameSystemManager_1.gameSystemManager.getSystem("GameLogicManager");
            this.uiManager = GameSystemManager_1.gameSystemManager.getSystem("UIManager");
            this.lootSystem = GameSystemManager_1.gameSystemManager.getSystem("LootSystem");
            this.isSystemsInitialized = true;
            console.log("TS_Debug: 新架构系统初始化成功");
        }
        catch (error) {
            console.error("TS_Debug: 初始化新架构系统失败:", error);
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
        // 状态信息由C++层的StateManagerComponent管理
        stateText += "TS: State managed by C++ StateManagerComponent";
        // 原有系统状态信息
        if (TS_Debug.PlayerState) {
            const stateNum = TS_Debug.PlayerState.GetCurrentState();
            const legacyState = TS_Debug.EPlayerStateMap[stateNum] ?? stateNum.toString();
            stateText = `${legacyState}`;
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
            return "攻击系统: 未初始化";
        }
        const attackStats = this.attackSystem.getAttackStats();
        return `攻击: 连击=${attackStats.attackComboCount}, 上次攻击=${attackStats.timeSinceLastAttack}ms前`;
    }
    // 获取事件系统信息
    Get_Event_System_Info() {
        if (!this.isSystemsInitialized) {
            return "事件系统: 未初始化";
        }
        const registeredEvents = EventSystem_1.EventSystem.getRegisteredEvents();
        const recentEvents = EventSystem_1.EventSystem.getEventHistory(undefined, 3);
        let info = `事件: ${registeredEvents.length}种类型`;
        if (recentEvents.length > 0) {
            const recentEventNames = recentEvents.map(e => e.event).join(", ");
            info += ` | 最近: ${recentEventNames}`;
        }
        return info;
    }
    // 获取掉落系统信息
    Get_Loot_System_Info() {
        if (!this.isSystemsInitialized || !this.lootSystem) {
            return "掉落系统: 未初始化";
        }
        const dropStats = this.lootSystem.getDropStats();
        return `掉落: ${dropStats.totalDrops}次掉落 | 最近: ${dropStats.recentDrops.length}`;
    }
    // 获取UI系统信息
    Get_UI_System_Info() {
        if (!this.isSystemsInitialized || !this.uiManager) {
            return "UI系统: 未初始化";
        }
        const uiStats = this.uiManager.getUIStats();
        return `UI: ${uiStats.activeDamageNumbers}个伤害数字, ${uiStats.activeNotifications}个通知`;
    }
    // 获取系统架构概览
    Get_System_Architecture_Info() {
        if (!this.isSystemsInitialized) {
            return "新架构: 未初始化";
        }
        const systemStatus = GameSystemManager_1.gameSystemManager.getSystemStatus();
        return `架构: ${systemStatus.systemCount}个系统活跃, ${systemStatus.eventStats.registeredEvents}种事件类型`;
    }
    // 测试新架构功能的按钮事件
    // 测试攻击系统 - 直接调用 C++ 攻击系统
    Test_Attack_System() {
        console.log("测试攻击系统 (C++ 直接调用)");
        if (this.isSystemsInitialized && TS_Debug.pawn) {
            try {
                // 直接调用 C++ 攻击系统
                const gameSystemManager = UE.GameSystemManager?.GetInstance?.();
                if (gameSystemManager) {
                    const attackComponent = gameSystemManager.GetAttackSystem();
                    if (attackComponent) {
                        attackComponent.ProcessAttackInput(TS_Debug.pawn);
                        console.log("攻击系统测试: C++ 直接调用成功");
                    }
                    else {
                        console.error("攻击系统测试: C++ AttackSystem 未找到");
                    }
                }
                else {
                    console.error("攻击系统测试: GameSystemManager 不可用");
                }
            }
            catch (error) {
                console.error("攻击系统测试: 调用C++系统时出错:", error);
            }
        }
    }
    // 测试状态转换
    Test_State_Transition() {
        console.log("测试状态转换");
        if (this.isSystemsInitialized && TS_Debug.pawn) {
            // 通过事件系统请求C++层进行状态转换
            EventSystem_1.EventSystem.emit("onRequestStateChange", {
                actor: TS_Debug.pawn,
                requestedState: "Attack",
                source: "DebugUI"
            });
        }
    }
    // 测试掉落系统
    Test_Loot_System() {
        console.log("测试掉落系统");
        if (this.isSystemsInitialized && this.lootSystem && TS_Debug.pawn) {
            this.lootSystem.generateLoot(TS_Debug.pawn, "DestructibleItem", 1.0);
        }
    }
    // 测试伤害数字显示
    Test_Damage_Numbers() {
        console.log("测试伤害数字显示");
        if (this.isSystemsInitialized && this.uiManager && TS_Debug.pawn) {
            const playerLocation = TS_Debug.pawn.K2_GetActorLocation();
            const damage = Math.floor(Math.random() * 50) + 10;
            const isCritical = Math.random() > 0.7;
            this.uiManager.showDamageNumber(damage, playerLocation, isCritical);
        }
    }
    // 重置所有系统
    Reset_All_Systems() {
        console.log("重置所有系统");
        if (this.isSystemsInitialized) {
            // 通过事件系统请求C++层重置系统
            EventSystem_1.EventSystem.emit("onRequestSystemReset", {
                system: "All",
                source: "DebugUI"
            });
        }
    }
    // 打印系统详细信息到控制台
    Print_System_Details() {
        console.log("=== 系统详细信息 ===");
        if (this.isSystemsInitialized) {
            GameSystemManager_1.gameSystemManager.debugPrintStatus();
            console.log("攻击系统:", this.Get_Attack_System_Info());
            console.log("事件系统:", this.Get_Event_System_Info());
            console.log("掉落系统:", this.Get_Loot_System_Info());
            console.log("UI系统:", this.Get_UI_System_Info());
        }
        else {
            console.log("新架构系统未初始化");
        }
        console.log("======================");
    }
    // 获取完整的调试信息字符串
    Get_Complete_Debug_Info() {
        let debugInfo = "=== 完整调试信息 ===\\n";
        // 基础信息
        debugInfo += `状态: ${this.Get_State_Text()}\\n`;
        debugInfo += `区域ID: ${this.Get_ID_Text()}\\n`;
        // 新架构系统信息
        if (this.isSystemsInitialized) {
            debugInfo += `架构: ${this.Get_System_Architecture_Info()}\\n`;
            debugInfo += `攻击: ${this.Get_Attack_System_Info()}\\n`;
            debugInfo += `事件: ${this.Get_Event_System_Info()}\\n`;
            debugInfo += `掉落: ${this.Get_Loot_System_Info()}\\n`;
            debugInfo += `UI: ${this.Get_UI_System_Info()}\\n`;
        }
        else {
            debugInfo += "新架构: 未初始化\\n";
        }
        debugInfo += "===========================";
        return debugInfo;
    }
}
puerts_1.blueprint.mixin(jsClass, TS_Debug);
//# sourceMappingURL=TS_Debug.js.map