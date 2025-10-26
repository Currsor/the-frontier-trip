import { GameLogicManager } from "./Managers/GameLogicManager";
import { UIManager } from "./Managers/UIManager";
import { AttackSystem } from "./Systems/AttackSystem";
import { LootSystem } from "./Systems/LootSystem";
import { EventSystem } from "./Systems/EventSystem";
import { GameConfig } from "./Config/GameConfig";

/**
 * 游戏系统管理器
 * 统一管理所有游戏系统的初始化、更新和销毁
 */
export class GameSystemManager {
    private static instance: GameSystemManager;
    private isInitialized: boolean = false;
    private systems: Map<string, any> = new Map();

    public static getInstance(): GameSystemManager {
        if (!GameSystemManager.instance) {
            GameSystemManager.instance = new GameSystemManager();
        }
        return GameSystemManager.instance;
    }

    private constructor() {
        console.log("游戏系统管理器已创建");
    }

    /**
     * 初始化所有游戏系统
     */
    public initialize(): void {
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
            EventSystem.emit("onGameSystemsInitialized", {
                timestamp: Date.now()
            });

        } catch (error) {
            console.error("初始化游戏系统管理器失败:", error);
            throw error;
        }
    }

    /**
     * 初始化核心系统
     */
    private initializeCore(): void {
        console.log("正在初始化核心系统...");
        
        // 事件系统最先初始化
        this.systems.set("EventSystem", EventSystem);
        
        // 配置系统
        this.systems.set("GameConfig", GameConfig);
        
        console.log("核心系统已初始化");
    }

    /**
     * 初始化管理器
     */
    private initializeManagers(): void {
        console.log("正在初始化管理器...");
        
        // 游戏逻辑管理器
        const gameLogicManager = GameLogicManager.getInstance();
        this.systems.set("GameLogicManager", gameLogicManager);
        
        // UI管理器
        const uiManager = UIManager.getInstance();
        this.systems.set("UIManager", uiManager);
        
        console.log("管理器已初始化");
    }

    /**
     * 初始化系统
     */
    private initializeSystems(): void {
        console.log("正在初始化系统...");
        
        // 攻击系统代理（实际逻辑在 C++ 中）
        const attackSystem = AttackSystem.getInstance();
        this.systems.set("AttackSystem", attackSystem);
        console.log("攻击系统代理已初始化 - 所有逻辑在C++中处理");
        
        // 掉落系统
        const lootSystem = LootSystem.getInstance();
        this.systems.set("LootSystem", lootSystem);
        
        console.log("系统已初始化");
    }

    /**
     * 设置系统间连接
     */
    private setupSystemConnections(): void {
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
    private setupAttackSystemConnections(): void {
        // 攻击命中时触发UI效果（事件由 C++ 发出）
        EventSystem.subscribe("onAttackHit", (data: any) => {
            const uiManager = this.getSystem("UIManager") as UIManager;
            uiManager.showHitEffect(data.target, data.isCritical);
        });

        // 攻击开始时更新状态（事件由 C++ 发出）
        EventSystem.subscribe("onAttackStarted", (data: any) => {
            console.log(`攻击系统 (C++): ${data.attacker.GetName()} 开始攻击`);
        });
    }

    /**
     * 设置掉落系统连接
     */
    private setupLootSystemConnections(): void {
        // 物品被破坏时生成掉落
        EventSystem.subscribe("onItemDestroyed", (data: any) => {
            const lootSystem = this.getSystem("LootSystem") as LootSystem;
            // 这里会在LootSystem内部处理
        });

        // 掉落生成时显示UI通知
        EventSystem.subscribe("onLootGenerated", (data: any) => {
            const uiManager = this.getSystem("UIManager") as UIManager;
            uiManager.showLootNotification(data.items);
        });
    }

    /**
     * 设置UI连接
     */
    private setupUIConnections(): void {
        // 状态变化时更新UI
        EventSystem.subscribe("onStateChanged", (data: any) => {
            const uiManager = this.getSystem("UIManager") as UIManager;
            uiManager.updateStateDisplay(data.newState, data.oldState);
        });

        // 伤害应用时显示伤害数字
        EventSystem.subscribe("onDamageApplied", (data: any) => {
            const uiManager = this.getSystem("UIManager") as UIManager;
            uiManager.showDamageNumber(data.damage, data.target.K2_GetActorLocation(), false);
        });
    }

    /**
     * 获取指定系统
     * @param systemName 系统名称
     * @returns 系统实例
     */
    public getSystem(systemName: string): any {
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
    public isSystemInitialized(systemName: string): boolean {
        return this.systems.has(systemName);
    }

    /**
     * 获取所有系统名称
     * @returns 系统名称数组
     */
    public getSystemNames(): string[] {
        return Array.from(this.systems.keys());
    }

    /**
     * 更新所有系统（如果需要的话）
     * @param deltaTime 时间增量
     */
    public update(deltaTime: number): void {
        if (!this.isInitialized) {
            return;
        }

        // 这里可以添加需要每帧更新的系统逻辑
        // 大部分系统是事件驱动的，不需要每帧更新
    }

    /**
     * 重置所有系统
     */
    public resetAllSystems(): void {
        console.log("正在重置所有系统...");

        // 重置攻击系统（通过代理调用 C++ 重置）
        const attackSystem = this.getSystem("AttackSystem") as AttackSystem;
        if (attackSystem) {
            attackSystem.reset();
            console.log("攻击系统通过C++代理重置");
        }

        const lootSystem = this.getSystem("LootSystem") as LootSystem;
        if (lootSystem) {
            lootSystem.clearDropHistory();
        }

        const uiManager = this.getSystem("UIManager") as UIManager;
        if (uiManager) {
            uiManager.cleanup();
        }

        console.log("所有系统已重置");
    }

    /**
     * 销毁所有系统
     */
    public shutdown(): void {
        console.log("正在关闭游戏系统管理器...");

        // 清理UI资源
        const uiManager = this.getSystem("UIManager") as UIManager;
        if (uiManager) {
            uiManager.cleanup();
        }

        // 清除所有事件监听器
        EventSystem.clearAllListeners();

        // 清空系统映射
        this.systems.clear();
        this.isInitialized = false;

        console.log("游戏系统管理器关闭完成");
    }

    /**
     * 获取系统状态信息
     */
    public getSystemStatus(): any {
        const status = {
            isInitialized: this.isInitialized,
            systemCount: this.systems.size,
            systems: {} as any,
            eventStats: {
                registeredEvents: EventSystem.getRegisteredEvents().length,
                recentEvents: EventSystem.getEventHistory(undefined, 5)
            }
        };

        // 收集各系统状态
        this.systems.forEach((system, name) => {
            if (system && typeof system.getStats === 'function') {
                status.systems[name] = system.getStats();
            } else if (system && typeof system.getSystemStats === 'function') {
                status.systems[name] = system.getSystemStats();
            } else {
                status.systems[name] = { status: 'active' };
            }
        });

        return status;
    }

    /**
     * 调试信息输出
     */
    public debugPrintStatus(): void {
        console.log("=== 游戏系统管理器状态 ===");
        console.log(`已初始化: ${this.isInitialized}`);
        console.log(`系统数量: ${this.systems.size}`);
        this.systems.forEach((system, name) => {
            console.log(`- ${name}: ${system ? '活跃' : '未活跃'}`);
        });
        EventSystem.debugPrintListeners();
        console.log("==================================");
    }
}

// 导出单例实例以便全局访问
export const gameSystemManager = GameSystemManager.getInstance();