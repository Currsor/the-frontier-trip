"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.gameSystemManager = exports.GameSystemManager = void 0;
const GameLogicManager_1 = require("./Managers/GameLogicManager");
const UIManager_1 = require("./Managers/UIManager");
const AttackSystem_1 = require("./Systems/AttackSystem");
const LootSystem_1 = require("./Systems/LootSystem");
const EventSystem_1 = require("./Systems/EventSystem");
const GameConfig_1 = require("./Config/GameConfig");
const TS_CardAnimationSystem_1 = require("./UMG/Card/TS_CardAnimationSystem");
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
        console.log("游戏系统管理器已创建");
    }
    /**
     * 初始化所有游戏系统
     */
    initialize() {
        if (this.isInitialized) {
            console.log("游戏系统管理器已初始化");
            return;
        }
        console.log("正在初始化游戏系统管理器...");
        try {
            // 按依赖顺序初始化系统
            this.initializeCore();
            this.initializeManagers();
            this.initializeSystems();
            this.setupSystemConnections();
            this.isInitialized = true;
            console.log("游戏系统管理器初始化成功");
            // 广播初始化完成事件
            EventSystem_1.EventSystem.emit("onGameSystemsInitialized", {
                timestamp: Date.now()
            });
        }
        catch (error) {
            console.error("初始化游戏系统管理器失败:", error);
            throw error;
        }
    }
    /**
     * 初始化核心系统
     */
    initializeCore() {
        console.log("正在初始化核心系统...");
        // 事件系统最先初始化
        this.systems.set("EventSystem", EventSystem_1.EventSystem);
        // 配置系统
        this.systems.set("GameConfig", GameConfig_1.GameConfig);
        console.log("核心系统已初始化");
    }
    /**
     * 初始化管理器
     */
    initializeManagers() {
        console.log("正在初始化管理器...");
        // 游戏逻辑管理器
        const gameLogicManager = GameLogicManager_1.GameLogicManager.getInstance();
        this.systems.set("GameLogicManager", gameLogicManager);
        // UI管理器
        const uiManager = UIManager_1.UIManager.getInstance();
        this.systems.set("UIManager", uiManager);
        console.log("管理器已初始化");
    }
    /**
     * 初始化系统
     */
    initializeSystems() {
        console.log("正在初始化系统...");
        // 攻击系统代理（实际逻辑在 C++ 中）
        const attackSystem = AttackSystem_1.AttackSystem.getInstance();
        this.systems.set("AttackSystem", attackSystem);
        console.log("攻击系统代理已初始化 - 所有逻辑在C++中处理");
        // 掉落系统
        const lootSystem = LootSystem_1.LootSystem.getInstance();
        this.systems.set("LootSystem", lootSystem);
        // 卡牌动画管理器
        const cardAnimationManager = TS_CardAnimationSystem_1.CardAnimationManager.getInstance();
        this.systems.set("CardAnimationManager", cardAnimationManager);
        console.log("卡牌动画管理器已初始化");
        console.log("系统已初始化");
    }
    /**
     * 设置系统间连接
     */
    setupSystemConnections() {
        console.log("正在设置系统连接...");
        // 设置系统间的事件连接
        this.setupAttackSystemConnections();
        this.setupLootSystemConnections();
        this.setupUIConnections();
        console.log("系统连接已建立");
    }
    /**
     * 设置攻击系统连接
     * 注意：攻击逻辑完全在 C++ 中处理，这里只处理 UI 响应
     */
    setupAttackSystemConnections() {
        // 攻击命中时触发UI效果（事件由 C++ 发出）
        EventSystem_1.EventSystem.subscribe("onAttackHit", (data) => {
            const uiManager = this.getSystem("UIManager");
            uiManager.showHitEffect(data.target, data.isCritical);
        });
        // 攻击开始时更新状态（事件由 C++ 发出）
        EventSystem_1.EventSystem.subscribe("onAttackStarted", (data) => {
            console.log(`攻击系统 (C++): ${data.attacker.GetName()} 开始攻击`);
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
            console.error(`系统未找到: ${systemName}`);
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
        console.log("正在重置所有系统...");
        // 重置攻击系统（通过代理调用 C++ 重置）
        const attackSystem = this.getSystem("AttackSystem");
        if (attackSystem) {
            attackSystem.reset();
            console.log("攻击系统通过C++代理重置");
        }
        const lootSystem = this.getSystem("LootSystem");
        if (lootSystem) {
            lootSystem.clearDropHistory();
        }
        const uiManager = this.getSystem("UIManager");
        if (uiManager) {
            uiManager.cleanup();
        }
        const cardAnimationManager = this.getSystem("CardAnimationManager");
        if (cardAnimationManager) {
            cardAnimationManager.cleanupAll();
            console.log("卡牌动画管理器已清理");
        }
        console.log("所有系统已重置");
    }
    /**
     * 销毁所有系统
     */
    shutdown() {
        console.log("正在关闭游戏系统管理器...");
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
        console.log("游戏系统管理器关闭完成");
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
        console.log("=== 游戏系统管理器状态 ===");
        console.log(`已初始化: ${this.isInitialized}`);
        console.log(`系统数量: ${this.systems.size}`);
        this.systems.forEach((system, name) => {
            console.log(`- ${name}: ${system ? '活跃' : '未活跃'}`);
        });
        EventSystem_1.EventSystem.debugPrintListeners();
        console.log("==================================");
    }
}
exports.GameSystemManager = GameSystemManager;
// 导出单例实例以便全局访问
exports.gameSystemManager = GameSystemManager.getInstance();
//# sourceMappingURL=GameSystemManager.js.map