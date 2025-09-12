"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.gameSystemManager = exports.GameSystemManager = void 0;
const GameLogicManager_1 = require("./Managers/GameLogicManager");
const StateManager_1 = require("./Managers/StateManager");
const UIManager_1 = require("./Managers/UIManager");
const AttackSystem_1 = require("./Systems/AttackSystem");
const LootSystem_1 = require("./Systems/LootSystem");
const EventSystem_1 = require("./Systems/EventSystem");
const GameConfig_1 = require("./Config/GameConfig");
/**
 * 游戏系统管理器
 * 统一管理所有游戏系统的初始化、更新和销毁
 */
class GameSystemManager {
    static instance;
    isInitialized = false;
    systems = new Map();
    static getInstance() {
        if (!GameSystemManager.instance) {
            GameSystemManager.instance = new GameSystemManager();
        }
        return GameSystemManager.instance;
    }
    constructor() {
        console.log("GameSystemManager created");
    }
    /**
     * 初始化所有游戏系统
     */
    initialize() {
        if (this.isInitialized) {
            console.log("GameSystemManager already initialized");
            return;
        }
        console.log("Initializing GameSystemManager...");
        try {
            // 按依赖顺序初始化系统
            this.initializeCore();
            this.initializeManagers();
            this.initializeSystems();
            this.setupSystemConnections();
            this.isInitialized = true;
            console.log("GameSystemManager initialized successfully");
            // 广播初始化完成事件
            EventSystem_1.EventSystem.emit("onGameSystemsInitialized", {
                timestamp: Date.now()
            });
        }
        catch (error) {
            console.error("Failed to initialize GameSystemManager:", error);
            throw error;
        }
    }
    /**
     * 初始化核心系统
     */
    initializeCore() {
        console.log("Initializing core systems...");
        // 事件系统最先初始化
        this.systems.set("EventSystem", EventSystem_1.EventSystem);
        // 配置系统
        this.systems.set("GameConfig", GameConfig_1.GameConfig);
        console.log("Core systems initialized");
    }
    /**
     * 初始化管理器
     */
    initializeManagers() {
        console.log("Initializing managers...");
        // 游戏逻辑管理器
        const gameLogicManager = GameLogicManager_1.GameLogicManager.getInstance();
        this.systems.set("GameLogicManager", gameLogicManager);
        // 状态管理器
        const stateManager = StateManager_1.StateManager.getInstance();
        this.systems.set("StateManager", stateManager);
        // UI管理器
        const uiManager = UIManager_1.UIManager.getInstance();
        this.systems.set("UIManager", uiManager);
        console.log("Managers initialized");
    }
    /**
     * 初始化系统
     */
    initializeSystems() {
        console.log("Initializing systems...");
        // 攻击系统
        const attackSystem = AttackSystem_1.AttackSystem.getInstance();
        this.systems.set("AttackSystem", attackSystem);
        // 掉落系统
        const lootSystem = LootSystem_1.LootSystem.getInstance();
        this.systems.set("LootSystem", lootSystem);
        console.log("Systems initialized");
    }
    /**
     * 设置系统间连接
     */
    setupSystemConnections() {
        console.log("Setting up system connections...");
        // 设置系统间的事件连接
        this.setupAttackSystemConnections();
        this.setupLootSystemConnections();
        this.setupUIConnections();
        console.log("System connections established");
    }
    /**
     * 设置攻击系统连接
     */
    setupAttackSystemConnections() {
        // 攻击命中时触发UI效果
        EventSystem_1.EventSystem.subscribe("onAttackHit", (data) => {
            const uiManager = this.getSystem("UIManager");
            uiManager.showHitEffect(data.target, data.isCritical);
        });
        // 攻击开始时更新状态
        EventSystem_1.EventSystem.subscribe("onAttackStarted", (data) => {
            console.log(`Attack system: ${data.attacker.GetName()} started attacking`);
        });
    }
    /**
     * 设置掉落系统连接
     */
    setupLootSystemConnections() {
        // 物品被破坏时生成掉落
        EventSystem_1.EventSystem.subscribe("onItemDestroyed", (data) => {
            const lootSystem = this.getSystem("LootSystem");
            // 这里会在LootSystem内部处理
        });
        // 掉落生成时显示UI通知
        EventSystem_1.EventSystem.subscribe("onLootGenerated", (data) => {
            const uiManager = this.getSystem("UIManager");
            uiManager.showLootNotification(data.items);
        });
    }
    /**
     * 设置UI连接
     */
    setupUIConnections() {
        // 状态变化时更新UI
        EventSystem_1.EventSystem.subscribe("onStateChanged", (data) => {
            const uiManager = this.getSystem("UIManager");
            uiManager.updateStateDisplay(data.newState, data.oldState);
        });
        // 伤害应用时显示伤害数字
        EventSystem_1.EventSystem.subscribe("onDamageApplied", (data) => {
            const uiManager = this.getSystem("UIManager");
            uiManager.showDamageNumber(data.damage, data.target.K2_GetActorLocation(), false);
        });
    }
    /**
     * 获取指定系统
     * @param systemName 系统名称
     * @returns 系统实例
     */
    getSystem(systemName) {
        const system = this.systems.get(systemName);
        if (!system) {
            console.error(`System not found: ${systemName}`);
            return null;
        }
        return system;
    }
    /**
     * 检查系统是否已初始化
     * @param systemName 系统名称
     * @returns 是否已初始化
     */
    isSystemInitialized(systemName) {
        return this.systems.has(systemName);
    }
    /**
     * 获取所有系统名称
     * @returns 系统名称数组
     */
    getSystemNames() {
        return Array.from(this.systems.keys());
    }
    /**
     * 更新所有系统（如果需要的话）
     * @param deltaTime 时间增量
     */
    update(deltaTime) {
        if (!this.isInitialized) {
            return;
        }
        // 这里可以添加需要每帧更新的系统逻辑
        // 大部分系统是事件驱动的，不需要每帧更新
    }
    /**
     * 重置所有系统
     */
    resetAllSystems() {
        console.log("Resetting all systems...");
        // 重置各个系统
        const attackSystem = this.getSystem("AttackSystem");
        if (attackSystem) {
            attackSystem.reset();
        }
        const stateManager = this.getSystem("StateManager");
        if (stateManager) {
            stateManager.reset();
        }
        const lootSystem = this.getSystem("LootSystem");
        if (lootSystem) {
            lootSystem.clearDropHistory();
        }
        const uiManager = this.getSystem("UIManager");
        if (uiManager) {
            uiManager.cleanup();
        }
        console.log("All systems reset");
    }
    /**
     * 销毁所有系统
     */
    shutdown() {
        console.log("Shutting down GameSystemManager...");
        // 清理UI资源
        const uiManager = this.getSystem("UIManager");
        if (uiManager) {
            uiManager.cleanup();
        }
        // 清除所有事件监听器
        EventSystem_1.EventSystem.clearAllListeners();
        // 清空系统映射
        this.systems.clear();
        this.isInitialized = false;
        console.log("GameSystemManager shutdown complete");
    }
    /**
     * 获取系统状态信息
     */
    getSystemStatus() {
        const status = {
            isInitialized: this.isInitialized,
            systemCount: this.systems.size,
            systems: {},
            eventStats: {
                registeredEvents: EventSystem_1.EventSystem.getRegisteredEvents().length,
                recentEvents: EventSystem_1.EventSystem.getEventHistory(undefined, 5)
            }
        };
        // 收集各系统状态
        this.systems.forEach((system, name) => {
            if (system && typeof system.getStats === 'function') {
                status.systems[name] = system.getStats();
            }
            else if (system && typeof system.getSystemStats === 'function') {
                status.systems[name] = system.getSystemStats();
            }
            else {
                status.systems[name] = { status: 'active' };
            }
        });
        return status;
    }
    /**
     * 调试信息输出
     */
    debugPrintStatus() {
        console.log("=== Game System Manager Status ===");
        console.log(`Initialized: ${this.isInitialized}`);
        console.log(`Systems Count: ${this.systems.size}`);
        this.systems.forEach((system, name) => {
            console.log(`- ${name}: ${system ? 'Active' : 'Inactive'}`);
        });
        EventSystem_1.EventSystem.debugPrintListeners();
        console.log("==================================");
    }
}
exports.GameSystemManager = GameSystemManager;
// 导出单例实例以便全局访问
exports.gameSystemManager = GameSystemManager.getInstance();
//# sourceMappingURL=GameSystemManager.js.map