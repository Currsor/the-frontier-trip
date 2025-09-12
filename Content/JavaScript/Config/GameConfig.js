"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.GameConfig = void 0;
/**
 * 游戏配置管理器
 * 集中管理所有游戏相关的配置参数和常量
 */
class GameConfig {
    // 伤害相关配置
    static DAMAGE_MULTIPLIERS = {
        NORMAL: 1.0,
        CRITICAL: 2.0,
        WEAK: 0.5,
        SUPER_EFFECTIVE: 1.5
    };
    static MIN_DAMAGE = 1;
    static MAX_HEALTH = 1000;
    static LEVEL_DAMAGE_MULTIPLIER = 0.1; // 每级增加10%伤害
    // 攻击系统配置
    static ATTACK_COOLDOWN = 0.5; // 秒
    static DEFAULT_ATTACK_RANGE = 200; // 单位
    static CRITICAL_HIT_BASE_CHANCE = 0.1; // 10%基础暴击率
    // 物品掉落配置
    static ITEM_DROP_RATES = {
        COMMON: 0.7, // 70%
        UNCOMMON: 0.2, // 20%
        RARE: 0.08, // 8%
        EPIC: 0.02 // 2%
    };
    static DROP_RADIUS = 100; // 掉落物品散布半径
    // 状态系统配置
    static STATE_PRIORITIES = {
        Dead: 100,
        Hurt: 90,
        Dash: 80,
        Attack: 70,
        RunAttack: 65,
        Jump: 60,
        Fall: 55,
        Run: 30,
        Walk: 20,
        Idle: 10
    };
    // 移动相关配置
    static MOVEMENT_THRESHOLDS = {
        WALK_THRESHOLD: 10.0,
        RUN_THRESHOLD: 700.0,
        RUN_ATTACK_THRESHOLD: 10.0
    };
    // 生命值系统配置
    static HEALTH_CONFIG = {
        PLAYER_MAX_HEALTH: 100,
        ENEMY_BASE_HEALTH: 50,
        DESTRUCTIBLE_ITEM_HEALTH: 50,
        HEALTH_REGEN_RATE: 1.0, // 每秒恢复
        HEALTH_REGEN_DELAY: 5.0 // 受伤后延迟恢复时间
    };
    // 经验值系统配置
    static EXP_CONFIG = {
        BASE_EXP_GAIN: 50,
        LEVEL_EXP_MULTIPLIER: 0.2,
        MIN_EXP_MULTIPLIER: 0.1,
        MAX_LEVEL_DIFFERENCE: 10
    };
    // 音效和特效配置
    static EFFECT_CONFIG = {
        DAMAGE_NUMBER_DURATION: 2.0,
        SCREEN_SHAKE_INTENSITY: 0.5,
        HIT_STOP_DURATION: 0.1
    };
    // UI配置
    static UI_CONFIG = {
        DAMAGE_TEXT_FADE_TIME: 1.5,
        HEALTH_BAR_UPDATE_SPEED: 2.0,
        NOTIFICATION_DISPLAY_TIME: 3.0
    };
    // 调试配置
    static DEBUG_CONFIG = {
        SHOW_COLLISION_BOXES: false,
        SHOW_DAMAGE_NUMBERS: true,
        LOG_ATTACK_DETAILS: true,
        SHOW_STATE_TRANSITIONS: false
    };
    // 性能配置
    static PERFORMANCE_CONFIG = {
        MAX_DAMAGE_NUMBERS: 10,
        PARTICLE_POOL_SIZE: 50,
        SOUND_POOL_SIZE: 20
    };
    /**
     * 获取状态优先级
     * @param state 状态名称
     * @returns 优先级数值（越高优先级越高）
     */
    static getStatePriority(state) {
        return this.STATE_PRIORITIES[state] || 0;
    }
    /**
     * 检查状态是否可以转换
     * @param fromState 当前状态
     * @param toState 目标状态
     * @returns 是否可以转换
     */
    static canTransitionState(fromState, toState) {
        const fromPriority = this.getStatePriority(fromState);
        const toPriority = this.getStatePriority(toState);
        // 高优先级状态可以打断低优先级状态
        return toPriority >= fromPriority;
    }
    /**
     * 根据稀有度获取掉落概率
     * @param rarity 稀有度
     * @returns 掉落概率
     */
    static getDropRate(rarity) {
        return this.ITEM_DROP_RATES[rarity.toUpperCase()] || 0;
    }
    /**
     * 获取等级对应的经验值需求
     * @param level 等级
     * @returns 升级所需经验值
     */
    static getExpRequirement(level) {
        return Math.floor(100 * Math.pow(1.5, level - 1));
    }
    /**
     * 验证配置值是否在有效范围内
     * @param configKey 配置键
     * @param value 值
     * @returns 是否有效
     */
    static validateConfig(configKey, value) {
        switch (configKey) {
            case "health":
                return value > 0 && value <= this.MAX_HEALTH;
            case "damage":
                return value >= this.MIN_DAMAGE;
            case "dropRate":
                return value >= 0 && value <= 1;
            default:
                return true;
        }
    }
}
exports.GameConfig = GameConfig;
//# sourceMappingURL=GameConfig.js.map