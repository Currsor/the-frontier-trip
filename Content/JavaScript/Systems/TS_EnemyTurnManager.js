"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.TS_EnemyTurnManager = void 0;
const EventSystem_1 = require("./EventSystem");
/** 对应 C++ EEnemyAttackType 枚举值 */
var EEnemyAttackType;
(function (EEnemyAttackType) {
    EEnemyAttackType[EEnemyAttackType["NormalAttack"] = 0] = "NormalAttack";
    EEnemyAttackType[EEnemyAttackType["HeavyAttack"] = 1] = "HeavyAttack";
    EEnemyAttackType[EEnemyAttackType["Defend"] = 2] = "Defend";
    EEnemyAttackType[EEnemyAttackType["SkillAttack"] = 3] = "SkillAttack";
})(EEnemyAttackType || (EEnemyAttackType = {}));
/** 敌人回合管理器 —— 单例，负责控制敌人回合流程 */
class TS_EnemyTurnManager {
    static instance = null;
    /** 当前战斗中的敌人列表（由外部注册） */
    enemies = [];
    /** 敌人行动之间的间隔（毫秒） */
    actionInterval = 800;
    constructor() {
        // 订阅玩家结束回合事件
        EventSystem_1.EventSystem.subscribe('EndTurn', this.onEndTurn.bind(this));
        console.log('[EnemyTurnManager] 初始化完成，已订阅 EndTurn 事件');
    }
    /** 获取单例 */
    static getInstance() {
        if (!TS_EnemyTurnManager.instance) {
            TS_EnemyTurnManager.instance = new TS_EnemyTurnManager();
        }
        return TS_EnemyTurnManager.instance;
    }
    /** 销毁单例（关卡切换时调用） */
    static destroyInstance() {
        if (TS_EnemyTurnManager.instance) {
            EventSystem_1.EventSystem.unsubscribe('EndTurn', TS_EnemyTurnManager.instance.onEndTurn.bind(TS_EnemyTurnManager.instance));
            TS_EnemyTurnManager.instance = null;
            console.log('[EnemyTurnManager] 单例已销毁');
        }
    }
    // ─────────────────────────────────────────────
    // 敌人注册
    // ─────────────────────────────────────────────
    /**
     * 注册参与战斗的敌人
     * @param enemy BaseEnemy 实例
     */
    registerEnemy(enemy) {
        if (!this.enemies.includes(enemy)) {
            this.enemies.push(enemy);
            console.log(`[EnemyTurnManager] 注册敌人: ${enemy.GetName ? enemy.GetName() : '未知'}`);
        }
    }
    /**
     * 注销敌人（死亡时调用）
     * @param enemy BaseEnemy 实例
     */
    unregisterEnemy(enemy) {
        const idx = this.enemies.indexOf(enemy);
        if (idx !== -1) {
            this.enemies.splice(idx, 1);
            console.log(`[EnemyTurnManager] 注销敌人: ${enemy.GetName ? enemy.GetName() : '未知'}`);
        }
    }
    /** 清空所有敌人（战斗结束时调用） */
    clearEnemies() {
        this.enemies = [];
        console.log('[EnemyTurnManager] 已清空敌人列表');
    }
    // ─────────────────────────────────────────────
    // 回合流程
    // ─────────────────────────────────────────────
    /** 玩家结束回合时触发 */
    onEndTurn(_data) {
        console.log('[EnemyTurnManager] ========== 敌人回合开始 ==========');
        // 通知 UI 禁用玩家操作
        EventSystem_1.EventSystem.emit('EnemyTurnStart', {});
        // 依次执行每个敌人的行动
        this.executeEnemyActions(0);
    }
    /**
     * 递归依次执行敌人行动
     * @param index 当前执行的敌人索引
     */
    executeEnemyActions(index) {
        // 过滤掉已死亡的敌人
        const aliveEnemies = this.enemies.filter(e => {
            if (e.IsDead && e.IsDead())
                return false;
            return true;
        });
        if (index >= aliveEnemies.length) {
            // 所有敌人行动完毕，回到玩家回合
            this.onEnemyTurnEnd();
            return;
        }
        const enemy = aliveEnemies[index];
        this.performEnemyAction(enemy);
        // 等待间隔后执行下一个敌人
        setTimeout(() => {
            this.executeEnemyActions(index + 1);
        }, this.actionInterval);
    }
    /**
     * 执行单个敌人的行动
     * @param enemy BaseEnemy 实例
     */
    performEnemyAction(enemy) {
        const name = enemy.GetName ? enemy.GetName() : '未知敌人';
        // 读取 C++ 中配置的攻击类型
        const attackType = enemy.GetAttackType
            ? enemy.GetAttackType()
            : EEnemyAttackType.NormalAttack;
        // 读取 C++ 中配置的攻击伤害
        const damage = enemy.GetAttackDamage
            ? enemy.GetAttackDamage()
            : 10;
        console.log(`[EnemyTurnManager] 敌人 ${name} 行动，攻击类型: ${EEnemyAttackType[attackType]}，伤害: ${damage}`);
        switch (attackType) {
            case EEnemyAttackType.NormalAttack:
                this.doNormalAttack(enemy, damage);
                break;
            case EEnemyAttackType.HeavyAttack:
                this.doHeavyAttack(enemy, damage);
                break;
            case EEnemyAttackType.Defend:
                this.doDefend(enemy);
                break;
            case EEnemyAttackType.SkillAttack:
                this.doSkillAttack(enemy, damage);
                break;
            default:
                this.doNormalAttack(enemy, damage);
                break;
        }
    }
    // ─────────────────────────────────────────────
    // 各攻击类型实现
    // ─────────────────────────────────────────────
    /** 普通攻击：对玩家造成固定伤害 */
    doNormalAttack(enemy, damage) {
        console.log(`[EnemyTurnManager] 普通攻击，伤害: ${damage}`);
        EventSystem_1.EventSystem.emit('EnemyAttackPlayer', {
            enemy,
            attackType: EEnemyAttackType.NormalAttack,
            damage,
        });
    }
    /** 强力攻击：伤害 x1.5，但有前摇提示 */
    doHeavyAttack(enemy, damage) {
        const heavyDamage = Math.floor(damage * 1.5);
        console.log(`[EnemyTurnManager] 强力攻击，伤害: ${heavyDamage}`);
        EventSystem_1.EventSystem.emit('EnemyAttackPlayer', {
            enemy,
            attackType: EEnemyAttackType.HeavyAttack,
            damage: heavyDamage,
        });
    }
    /** 防御：本回合减少受到的伤害，通知玩家 UI 显示盾牌图标 */
    doDefend(enemy) {
        console.log(`[EnemyTurnManager] 敌人进入防御状态`);
        EventSystem_1.EventSystem.emit('EnemyDefend', { enemy });
    }
    /** 技能攻击：施加特殊状态效果（如中毒、眩晕等） */
    doSkillAttack(enemy, damage) {
        console.log(`[EnemyTurnManager] 技能攻击，伤害: ${damage}`);
        EventSystem_1.EventSystem.emit('EnemySkillAttack', {
            enemy,
            attackType: EEnemyAttackType.SkillAttack,
            damage,
        });
    }
    // ─────────────────────────────────────────────
    // 回合结束
    // ─────────────────────────────────────────────
    /** 敌人回合结束，切换回玩家回合 */
    onEnemyTurnEnd() {
        console.log('[EnemyTurnManager] ========== 敌人回合结束，玩家回合开始 ==========');
        EventSystem_1.EventSystem.emit('PlayerTurnStart', {});
    }
}
exports.TS_EnemyTurnManager = TS_EnemyTurnManager;
//# sourceMappingURL=TS_EnemyTurnManager.js.map