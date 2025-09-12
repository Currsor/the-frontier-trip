"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.UIManager = void 0;
const EventSystem_1 = require("../Systems/EventSystem");
const GameConfig_1 = require("../Config/GameConfig");
/**
 * UI管理器
 * 处理用户界面逻辑、状态更新和交互
 */
class UIManager {
    static instance;
    healthBarUpdateSpeed = GameConfig_1.GameConfig.UI_CONFIG.HEALTH_BAR_UPDATE_SPEED;
    damageNumberPool = [];
    activeNotifications = [];
    static getInstance() {
        if (!UIManager.instance) {
            UIManager.instance = new UIManager();
        }
        return UIManager.instance;
    }
    constructor() {
        this.setupEventListeners();
        this.initializeDamageNumberPool();
        console.log("UIManager initialized");
    }
    /**
     * 设置事件监听器
     */
    setupEventListeners() {
        EventSystem_1.EventSystem.subscribe("onHealthChanged", (data) => {
            this.updateHealthBar(data.currentHealth, data.maxHealth, data.target);
        });
        EventSystem_1.EventSystem.subscribe("onShowDamageNumber", (data) => {
            this.showDamageNumber(data.damage, data.position, data.isCritical);
        });
        EventSystem_1.EventSystem.subscribe("onStateChanged", (data) => {
            this.updateStateDisplay(data.newState, data.oldState);
        });
        EventSystem_1.EventSystem.subscribe("onLootGenerated", (data) => {
            this.showLootNotification(data.items);
        });
        EventSystem_1.EventSystem.subscribe("onAttackHit", (data) => {
            this.showHitEffect(data.target, data.isCritical);
        });
        EventSystem_1.EventSystem.subscribe("onExpGained", (data) => {
            this.updateExpBar(data.currentExp, data.maxExp, data.gainedExp);
        });
    }
    /**
     * 初始化伤害数字对象池
     */
    initializeDamageNumberPool() {
        for (let i = 0; i < GameConfig_1.GameConfig.PERFORMANCE_CONFIG.MAX_DAMAGE_NUMBERS; i++) {
            this.damageNumberPool.push({
                isActive: false,
                element: null,
                startTime: 0
            });
        }
    }
    /**
     * 更新生命值条
     * @param currentHealth 当前生命值
     * @param maxHealth 最大生命值
     * @param target 目标角色
     */
    updateHealthBar(currentHealth, maxHealth, target) {
        const healthPercentage = Math.max(0, currentHealth / maxHealth);
        console.log(`Updating health bar: ${currentHealth}/${maxHealth} (${(healthPercentage * 100).toFixed(1)}%)`);
        // 广播UI更新事件给蓝图
        EventSystem_1.EventSystem.emit("onUpdateHealthBarUI", {
            healthPercentage: healthPercentage,
            currentHealth: currentHealth,
            maxHealth: maxHealth,
            target: target,
            timestamp: Date.now()
        });
        // 检查低生命值警告
        if (healthPercentage < 0.25) {
            this.showLowHealthWarning();
        }
    }
    /**
     * 显示伤害数字
     * @param damage 伤害值
     * @param position 显示位置
     * @param isCritical 是否暴击
     */
    showDamageNumber(damage, position, isCritical = false) {
        const damageNumber = this.getDamageNumberFromPool();
        if (!damageNumber) {
            console.log("Damage number pool exhausted");
            return;
        }
        damageNumber.isActive = true;
        damageNumber.startTime = Date.now();
        // 广播伤害数字显示事件
        EventSystem_1.EventSystem.emit("onDisplayDamageNumber", {
            damage: damage,
            position: position,
            isCritical: isCritical,
            duration: GameConfig_1.GameConfig.UI_CONFIG.DAMAGE_TEXT_FADE_TIME,
            poolIndex: this.damageNumberPool.indexOf(damageNumber)
        });
        // 设置自动回收
        setTimeout(() => {
            this.returnDamageNumberToPool(damageNumber);
        }, GameConfig_1.GameConfig.UI_CONFIG.DAMAGE_TEXT_FADE_TIME * 1000);
        console.log(`Showing damage number: ${damage}${isCritical ? ' (CRITICAL!)' : ''}`);
    }
    /**
     * 从对象池获取伤害数字
     * @returns 伤害数字对象
     */
    getDamageNumberFromPool() {
        return this.damageNumberPool.find(item => !item.isActive);
    }
    /**
     * 将伤害数字返回对象池
     * @param damageNumber 伤害数字对象
     */
    returnDamageNumberToPool(damageNumber) {
        damageNumber.isActive = false;
        damageNumber.startTime = 0;
    }
    /**
     * 更新状态显示
     * @param newState 新状态
     * @param oldState 旧状态
     */
    updateStateDisplay(newState, oldState) {
        console.log(`State display updated: ${oldState} -> ${newState}`);
        // 广播状态UI更新事件
        EventSystem_1.EventSystem.emit("onUpdateStateUI", {
            newState: newState,
            oldState: oldState,
            timestamp: Date.now()
        });
        // 特殊状态的UI处理
        switch (newState) {
            case "Dead":
                this.showDeathScreen();
                break;
            case "Hurt":
                this.showHurtEffect();
                break;
            case "Attack":
            case "RunAttack":
                this.showAttackIndicator();
                break;
        }
    }
    /**
     * 显示击中效果
     * @param target 目标
     * @param isCritical 是否暴击
     */
    showHitEffect(target, isCritical) {
        // 屏幕闪烁效果
        EventSystem_1.EventSystem.emit("onScreenFlash", {
            color: isCritical ? "red" : "white",
            intensity: isCritical ? 0.8 : 0.3,
            duration: 0.1
        });
        // 击中标记
        EventSystem_1.EventSystem.emit("onShowHitMarker", {
            target: target,
            isCritical: isCritical,
            duration: 0.5
        });
        console.log(`Hit effect shown for ${target.GetName()}${isCritical ? ' (CRITICAL!)' : ''}`);
    }
    /**
     * 显示掉落通知
     * @param items 掉落物品列表
     */
    showLootNotification(items) {
        if (items.length === 0)
            return;
        const notification = {
            id: Date.now(),
            type: "loot",
            items: items,
            startTime: Date.now(),
            duration: GameConfig_1.GameConfig.UI_CONFIG.NOTIFICATION_DISPLAY_TIME
        };
        this.activeNotifications.push(notification);
        // 广播掉落通知事件
        EventSystem_1.EventSystem.emit("onShowLootNotification", {
            items: items,
            notificationId: notification.id,
            duration: notification.duration
        });
        // 自动移除通知
        setTimeout(() => {
            this.removeNotification(notification.id);
        }, notification.duration * 1000);
        console.log(`Loot notification shown: ${items.join(", ")}`);
    }
    /**
     * 更新经验值条
     * @param currentExp 当前经验值
     * @param maxExp 升级所需经验值
     * @param gainedExp 获得的经验值
     */
    updateExpBar(currentExp, maxExp, gainedExp) {
        const expPercentage = currentExp / maxExp;
        console.log(`Updating exp bar: ${currentExp}/${maxExp} (+${gainedExp})`);
        // 广播经验值UI更新事件
        EventSystem_1.EventSystem.emit("onUpdateExpBarUI", {
            expPercentage: expPercentage,
            currentExp: currentExp,
            maxExp: maxExp,
            gainedExp: gainedExp,
            timestamp: Date.now()
        });
        // 检查升级
        if (currentExp >= maxExp) {
            this.showLevelUpEffect();
        }
    }
    /**
     * 显示低生命值警告
     */
    showLowHealthWarning() {
        EventSystem_1.EventSystem.emit("onShowLowHealthWarning", {
            timestamp: Date.now()
        });
    }
    /**
     * 显示死亡界面
     */
    showDeathScreen() {
        EventSystem_1.EventSystem.emit("onShowDeathScreen", {
            timestamp: Date.now()
        });
        console.log("Death screen displayed");
    }
    /**
     * 显示受伤效果
     */
    showHurtEffect() {
        EventSystem_1.EventSystem.emit("onShowHurtEffect", {
            duration: 0.5,
            timestamp: Date.now()
        });
    }
    /**
     * 显示攻击指示器
     */
    showAttackIndicator() {
        EventSystem_1.EventSystem.emit("onShowAttackIndicator", {
            duration: 0.3,
            timestamp: Date.now()
        });
    }
    /**
     * 显示升级效果
     */
    showLevelUpEffect() {
        EventSystem_1.EventSystem.emit("onShowLevelUpEffect", {
            timestamp: Date.now()
        });
        console.log("Level up effect displayed");
    }
    /**
     * 移除通知
     * @param notificationId 通知ID
     */
    removeNotification(notificationId) {
        const index = this.activeNotifications.findIndex(n => n.id === notificationId);
        if (index > -1) {
            this.activeNotifications.splice(index, 1);
            EventSystem_1.EventSystem.emit("onRemoveNotification", {
                notificationId: notificationId
            });
        }
    }
    /**
     * 显示调试信息
     * @param info 调试信息
     */
    showDebugInfo(info) {
        if (!GameConfig_1.GameConfig.DEBUG_CONFIG.SHOW_STATE_TRANSITIONS) {
            return;
        }
        EventSystem_1.EventSystem.emit("onShowDebugInfo", {
            info: info,
            timestamp: Date.now()
        });
    }
    /**
     * 更新UI配置
     * @param config 配置对象
     */
    updateUIConfig(config) {
        if (config.healthBarUpdateSpeed !== undefined) {
            this.healthBarUpdateSpeed = config.healthBarUpdateSpeed;
        }
        console.log("UI config updated", config);
    }
    /**
     * 清理UI资源
     */
    cleanup() {
        // 清理活跃的伤害数字
        this.damageNumberPool.forEach(item => {
            item.isActive = false;
        });
        // 清理通知
        this.activeNotifications = [];
        console.log("UI resources cleaned up");
    }
    /**
     * 获取UI统计信息
     */
    getUIStats() {
        return {
            activeDamageNumbers: this.damageNumberPool.filter(item => item.isActive).length,
            activeNotifications: this.activeNotifications.length,
            poolSize: this.damageNumberPool.length
        };
    }
}
exports.UIManager = UIManager;
//# sourceMappingURL=UIManager.js.map