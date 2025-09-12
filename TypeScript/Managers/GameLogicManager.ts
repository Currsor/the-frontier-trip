import { Actor } from "ue";
import { GameConfig } from "../Config/GameConfig";
import { EventSystem } from "../Systems/EventSystem";

/**
 * 游戏逻辑管理器
 * 负责处理核心游戏逻辑、数据计算和业务规则
 */
export class GameLogicManager {
    private static instance: GameLogicManager;
    
    public static getInstance(): GameLogicManager {
        if (!GameLogicManager.instance) {
            GameLogicManager.instance = new GameLogicManager();
        }
        return GameLogicManager.instance;
    }

    private constructor() {
        console.log("GameLogicManager initialized");
    }

    /**
     * 计算攻击伤害
     * @param baseDamage 基础伤害
     * @param attackerLevel 攻击者等级
     * @param weaponMultiplier 武器倍率
     * @param isCritical 是否暴击
     * @returns 最终伤害值
     */
    public calculateDamage(
        baseDamage: number, 
        attackerLevel: number = 1, 
        weaponMultiplier: number = 1.0,
        isCritical: boolean = false
    ): number {
        let finalDamage = baseDamage * weaponMultiplier;
        
        // 等级加成
        finalDamage *= (1 + attackerLevel * GameConfig.LEVEL_DAMAGE_MULTIPLIER);
        
        // 暴击处理
        if (isCritical) {
            finalDamage *= GameConfig.DAMAGE_MULTIPLIERS.CRITICAL;
        }
        
        // 随机浮动 ±10%
        const randomFactor = 0.9 + Math.random() * 0.2;
        finalDamage *= randomFactor;
        
        return Math.max(1, Math.floor(finalDamage));
    }

    /**
     * 计算暴击概率
     * @param baseCritChance 基础暴击率
     * @param attackerLevel 攻击者等级
     * @returns 是否暴击
     */
    public calculateCriticalHit(baseCritChance: number = 0.1, attackerLevel: number = 1): boolean {
        const finalCritChance = baseCritChance + (attackerLevel * 0.01);
        return Math.random() < finalCritChance;
    }

    /**
     * 计算经验值获得
     * @param enemyLevel 敌人等级
     * @param playerLevel 玩家等级
     * @param baseExp 基础经验值
     * @returns 获得的经验值
     */
    public calculateExpGain(enemyLevel: number, playerLevel: number, baseExp: number = 50): number {
        const levelDiff = enemyLevel - playerLevel;
        let expMultiplier = 1.0;
        
        if (levelDiff > 0) {
            // 敌人等级高，额外经验
            expMultiplier = 1.0 + (levelDiff * 0.2);
        } else if (levelDiff < -5) {
            // 敌人等级太低，经验减少
            expMultiplier = Math.max(0.1, 1.0 + (levelDiff * 0.1));
        }
        
        return Math.floor(baseExp * expMultiplier);
    }

    /**
     * 验证攻击条件
     * @param attacker 攻击者
     * @param target 目标
     * @param attackRange 攻击范围
     * @returns 是否可以攻击
     */
    public validateAttackConditions(attacker: Actor, target: Actor, attackRange: number = 200): boolean {
        if (!attacker || !target) {
            return false;
        }

        // 检查距离
        const distance = attacker.GetDistanceTo(target);
        if (distance > attackRange) {
            console.log(`Attack failed: Target too far (${distance} > ${attackRange})`);
            return false;
        }

        // 检查目标是否有效
        if (!target || !(target as any).IsValidLowLevel()) {
            console.log("Attack failed: Invalid target");
            return false;
        }

        return true;
    }

    /**
     * 处理攻击结果
     * @param attacker 攻击者
     * @param target 目标
     * @param damage 伤害值
     * @param isCritical 是否暴击
     */
    public processAttackResult(attacker: Actor, target: Actor, damage: number, isCritical: boolean): void {
        // 广播攻击事件
        EventSystem.emit("onAttackHit", {
            attacker: attacker,
            target: target,
            damage: damage,
            isCritical: isCritical,
            timestamp: Date.now()
        });

        // 记录攻击统计
        this.recordAttackStats(attacker, damage, isCritical);

        console.log(`Attack processed: ${attacker.GetName()} -> ${target.GetName()}, Damage: ${damage}${isCritical ? ' (CRITICAL!)' : ''}`);
    }

    /**
     * 记录攻击统计
     * @param attacker 攻击者
     * @param damage 伤害值
     * @param isCritical 是否暴击
     */
    private recordAttackStats(attacker: Actor, damage: number, isCritical: boolean): void {
        // 这里可以记录到玩家统计数据中
        // 例如：总伤害、暴击次数等
        EventSystem.emit("onStatsUpdate", {
            type: "attack",
            damage: damage,
            critical: isCritical
        });
    }

    /**
     * 计算状态效果持续时间
     * @param baseTime 基础时间
     * @param resistance 抗性值 (0-1)
     * @returns 实际持续时间
     */
    public calculateStatusDuration(baseTime: number, resistance: number = 0): number {
        return baseTime * (1 - Math.min(0.9, resistance));
    }

    /**
     * 验证游戏规则
     * @param ruleType 规则类型
     * @param params 参数
     * @returns 是否符合规则
     */
    public validateGameRule(ruleType: string, params: any): boolean {
        switch (ruleType) {
            case "maxHealth":
                return params.health <= GameConfig.MAX_HEALTH;
            case "minDamage":
                return params.damage >= GameConfig.MIN_DAMAGE;
            case "attackCooldown":
                return params.timeSinceLastAttack >= GameConfig.ATTACK_COOLDOWN;
            default:
                return true;
        }
    }
}