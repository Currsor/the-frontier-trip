"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.LootSystem = void 0;
const GameConfig_1 = require("../Config/GameConfig");
const EventSystem_1 = require("../Systems/EventSystem");
/**
 * 掉落系统
 * 处理物品掉落逻辑、概率计算和掉落生成
 */
class LootSystem {
    static instance;
    lootTables = new Map();
    dropHistory = [];
    static getInstance() {
        if (!LootSystem.instance) {
            LootSystem.instance = new LootSystem();
        }
        return LootSystem.instance;
    }
    constructor() {
        this.initializeDefaultLootTables();
        this.setupEventListeners();
        console.log("LootSystem initialized");
    }
    /**
     * 初始化默认掉落表
     */
    initializeDefaultLootTables() {
        // 可破坏物品掉落表
        this.registerLootTable("DestructibleItem", {
            items: [
                {
                    itemClass: "Coin",
                    dropChance: 0.8,
                    minCount: 1,
                    maxCount: 3,
                    rarity: "COMMON"
                },
                {
                    itemClass: "HealthPotion",
                    dropChance: 0.3,
                    minCount: 1,
                    maxCount: 1,
                    rarity: "UNCOMMON"
                },
                {
                    itemClass: "MagicCrystal",
                    dropChance: 0.1,
                    minCount: 1,
                    maxCount: 1,
                    rarity: "RARE"
                }
            ]
        });
        // 敌人掉落表
        this.registerLootTable("Enemy", {
            items: [
                {
                    itemClass: "Coin",
                    dropChance: 0.9,
                    minCount: 2,
                    maxCount: 5,
                    rarity: "COMMON"
                },
                {
                    itemClass: "Weapon",
                    dropChance: 0.15,
                    minCount: 1,
                    maxCount: 1,
                    rarity: "UNCOMMON"
                }
            ],
            guaranteedDrops: [
                {
                    itemClass: "Experience",
                    dropChance: 1.0,
                    minCount: 10,
                    maxCount: 20,
                    rarity: "COMMON"
                }
            ]
        });
        // Boss掉落表
        this.registerLootTable("Boss", {
            items: [
                {
                    itemClass: "RareWeapon",
                    dropChance: 0.5,
                    minCount: 1,
                    maxCount: 1,
                    rarity: "RARE"
                },
                {
                    itemClass: "EpicArmor",
                    dropChance: 0.2,
                    minCount: 1,
                    maxCount: 1,
                    rarity: "EPIC"
                }
            ],
            guaranteedDrops: [
                {
                    itemClass: "BossKey",
                    dropChance: 1.0,
                    minCount: 1,
                    maxCount: 1,
                    rarity: "EPIC"
                }
            ]
        });
    }
    /**
     * 设置事件监听器
     */
    setupEventListeners() {
        EventSystem_1.EventSystem.subscribe("onItemDestroyed", (data) => {
            this.handleItemDestroyed(data.item);
        });
        EventSystem_1.EventSystem.subscribe("onEnemyDefeated", (data) => {
            this.handleEnemyDefeated(data.enemy);
        });
    }
    /**
     * 注册掉落表
     * @param tableId 掉落表ID
     * @param lootTable 掉落表配置
     */
    registerLootTable(tableId, lootTable) {
        this.lootTables.set(tableId, lootTable);
        console.log(`Loot table registered: ${tableId}`);
    }
    /**
     * 处理物品被破坏事件
     * @param item 被破坏的物品
     */
    handleItemDestroyed(item) {
        if (!item)
            return;
        const itemType = this.getItemType(item);
        this.generateLoot(item, itemType);
    }
    /**
     * 处理敌人被击败事件
     * @param enemy 被击败的敌人
     */
    handleEnemyDefeated(enemy) {
        if (!enemy)
            return;
        const enemyType = this.getEnemyType(enemy);
        this.generateLoot(enemy, enemyType);
    }
    /**
     * 生成掉落物品
     * @param source 掉落来源
     * @param lootTableId 掉落表ID
     * @param bonusMultiplier 奖励倍率
     */
    generateLoot(source, lootTableId, bonusMultiplier = 1.0) {
        const lootTable = this.lootTables.get(lootTableId);
        if (!lootTable) {
            console.log(`Loot table not found: ${lootTableId}`);
            return;
        }
        const droppedItems = [];
        const sourceLocation = source.K2_GetActorLocation();
        // 处理保证掉落
        if (lootTable.guaranteedDrops) {
            for (const guaranteedItem of lootTable.guaranteedDrops) {
                const count = this.calculateDropCount(guaranteedItem, bonusMultiplier);
                for (let i = 0; i < count; i++) {
                    this.spawnLootItem(guaranteedItem, sourceLocation, i);
                    droppedItems.push(guaranteedItem.itemClass);
                }
            }
        }
        // 处理概率掉落
        for (const lootItem of lootTable.items) {
            if (this.rollDropChance(lootItem, bonusMultiplier)) {
                const count = this.calculateDropCount(lootItem, bonusMultiplier);
                for (let i = 0; i < count; i++) {
                    this.spawnLootItem(lootItem, sourceLocation, i);
                    droppedItems.push(lootItem.itemClass);
                }
            }
        }
        // 处理条件掉落
        if (lootTable.conditionalDrops) {
            for (const conditionalDrop of lootTable.conditionalDrops) {
                if (this.checkDropCondition(conditionalDrop.condition, source)) {
                    for (const lootItem of conditionalDrop.items) {
                        if (this.rollDropChance(lootItem, bonusMultiplier)) {
                            const count = this.calculateDropCount(lootItem, bonusMultiplier);
                            for (let i = 0; i < count; i++) {
                                this.spawnLootItem(lootItem, sourceLocation, i);
                                droppedItems.push(lootItem.itemClass);
                            }
                        }
                    }
                }
            }
        }
        // 记录掉落历史
        this.recordDropHistory(source.GetName(), droppedItems);
        // 广播掉落事件
        EventSystem_1.EventSystem.emit("onLootGenerated", {
            source: source,
            items: droppedItems,
            location: sourceLocation,
            timestamp: Date.now()
        });
        console.log(`Loot generated from ${source.GetName()}: ${droppedItems.join(", ")}`);
    }
    /**
     * 计算掉落概率
     * @param lootItem 掉落物品
     * @param bonusMultiplier 奖励倍率
     * @returns 是否掉落
     */
    rollDropChance(lootItem, bonusMultiplier) {
        const finalChance = Math.min(1.0, lootItem.dropChance * bonusMultiplier);
        const roll = Math.random();
        console.log(`Drop roll for ${lootItem.itemClass}: ${roll.toFixed(3)} vs ${finalChance.toFixed(3)}`);
        return roll < finalChance;
    }
    /**
     * 计算掉落数量
     * @param lootItem 掉落物品
     * @param bonusMultiplier 奖励倍率
     * @returns 掉落数量
     */
    calculateDropCount(lootItem, bonusMultiplier) {
        const baseCount = Math.floor(Math.random() * (lootItem.maxCount - lootItem.minCount + 1)) + lootItem.minCount;
        return Math.max(1, Math.floor(baseCount * bonusMultiplier));
    }
    /**
     * 生成掉落物品实例
     * @param lootItem 掉落物品配置
     * @param sourceLocation 来源位置
     * @param index 索引（用于位置偏移）
     */
    spawnLootItem(lootItem, sourceLocation, index) {
        // 计算掉落位置
        const dropLocation = this.calculateDropPosition(sourceLocation, index);
        // 这里需要调用UE的生成Actor方法
        // 由于TypeScript限制，这里只是记录日志
        console.log(`Spawning ${lootItem.itemClass} at ${dropLocation.X}, ${dropLocation.Y}, ${dropLocation.Z}`);
        // 广播物品生成事件
        EventSystem_1.EventSystem.emit("onLootItemSpawned", {
            itemClass: lootItem.itemClass,
            location: dropLocation,
            rarity: lootItem.rarity,
            timestamp: Date.now()
        });
    }
    /**
     * 计算掉落位置
     * @param sourceLocation 来源位置
     * @param index 索引
     * @returns 掉落位置
     */
    calculateDropPosition(sourceLocation, index) {
        const angle = (index * 60) * (Math.PI / 180); // 每个物品间隔60度
        const radius = GameConfig_1.GameConfig.DROP_RADIUS + (index * 20); // 递增半径
        return {
            X: sourceLocation.X + Math.cos(angle) * radius,
            Y: sourceLocation.Y + Math.sin(angle) * radius,
            Z: sourceLocation.Z
        };
    }
    /**
     * 检查掉落条件
     * @param condition 条件字符串
     * @param source 掉落来源
     * @returns 是否满足条件
     */
    checkDropCondition(condition, source) {
        switch (condition) {
            case "firstKill":
                // 检查是否是首次击杀
                return true; // 简化实现
            case "lowHealth":
                // 检查玩家生命值是否较低
                return true; // 简化实现
            case "nightTime":
                // 检查是否是夜晚
                return true; // 简化实现
            default:
                return false;
        }
    }
    /**
     * 获取物品类型
     * @param item 物品Actor
     * @returns 物品类型
     */
    getItemType(item) {
        // 根据Actor类名或标签确定类型
        const className = item.GetClass().GetName();
        if (className.includes("Destructible")) {
            return "DestructibleItem";
        }
        return "DestructibleItem"; // 默认类型
    }
    /**
     * 获取敌人类型
     * @param enemy 敌人Actor
     * @returns 敌人类型
     */
    getEnemyType(enemy) {
        const className = enemy.GetClass().GetName();
        if (className.includes("Boss")) {
            return "Boss";
        }
        return "Enemy"; // 默认类型
    }
    /**
     * 记录掉落历史
     * @param sourceName 来源名称
     * @param items 掉落物品列表
     */
    recordDropHistory(sourceName, items) {
        this.dropHistory.push({
            source: sourceName,
            items: items,
            timestamp: Date.now()
        });
        // 限制历史记录大小
        if (this.dropHistory.length > 100) {
            this.dropHistory.shift();
        }
    }
    /**
     * 获取掉落统计
     * @param itemClass 物品类名（可选）
     * @returns 掉落统计信息
     */
    getDropStats(itemClass) {
        const stats = {
            totalDrops: 0,
            itemCounts: new Map(),
            recentDrops: this.dropHistory.slice(-10)
        };
        for (const record of this.dropHistory) {
            for (const item of record.items) {
                if (!itemClass || item === itemClass) {
                    stats.totalDrops++;
                    stats.itemCounts.set(item, (stats.itemCounts.get(item) || 0) + 1);
                }
            }
        }
        return {
            totalDrops: stats.totalDrops,
            itemCounts: Object.fromEntries(stats.itemCounts),
            recentDrops: stats.recentDrops
        };
    }
    /**
     * 清除掉落历史
     */
    clearDropHistory() {
        this.dropHistory = [];
        console.log("Drop history cleared");
    }
    /**
     * 修改掉落倍率
     * @param source 来源
     * @param multiplier 倍率
     */
    setDropMultiplier(source, multiplier) {
        // 这里可以实现临时的掉落倍率修改
        EventSystem_1.EventSystem.emit("onDropMultiplierChanged", {
            source: source,
            multiplier: multiplier
        });
    }
}
exports.LootSystem = LootSystem;
//# sourceMappingURL=LootSystem.js.map