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

    // 注意：所有攻击相关的计算（伤害、暴击等）现在完全在 C++ AttackSystemComponent 中处理
    // TypeScript 不再包含任何攻击逻辑

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
     * 计算状态效果持续时间
     * @param baseTime 基础时间
     * @param resistance 抗性值 (0-1)
     * @returns 实际持续时间
     */
    public calculateStatusDuration(baseTime: number, resistance: number = 0): number {
        return baseTime * (1 - Math.min(0.9, resistance));
    }

    /**
     * 验证非攻击相关的游戏规则
     * 注意：攻击相关规则验证现在在 C++ AttackSystemComponent 中处理
     */
    public validateGameRule(ruleType: string, params: any): boolean {
        switch (ruleType) {
            case "maxHealth":
                return params.health <= GameConfig.MAX_HEALTH;
            // 攻击相关规则已移至 C++
            default:
                return true;
        }
    }
}